/*
 * Generic HID++ definitions and helpers.
 *
 * Copyright (C) 2014 Peter Wu <peter@lekensteyn.nl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fcntl.h>
#include <glob.h>
#include <libgen.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <time.h> /* needs -lrt, for clock_gettime as timeout helper */
#include <unistd.h>
#include "hidpp.h"
#include "debug.h"

#define VID_LOGITECH        0x046d
#define PID_UNIFYING        0xc52b
#define PID_NANO_RECEIVER   0xc52f
#define RECEIVER_NAME "logitech-djreceiver"

static inline bool is_valid_device_index(uint8_t ix)
{
	return ix >= 1 && ix <= MAX_DEVICES;
}

static long long unsigned get_timestamp_ms(void)
{
	struct timespec tp;

	clock_gettime(CLOCK_MONOTONIC, &tp);
	return tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
}

static ssize_t poll_read(int fd, int timeout, void *buf, size_t count)
{
	int r;
	struct pollfd pollfd = {
		.fd = fd,
		.events = POLLIN
	};

	r = poll(&pollfd, 1, timeout);
	if (r < 0) {
		perror("poll");
		return 0;
	} else if (r == 0) {
		trace_log("poll timeout reached!\n");
		return 0;
	}

	r = read(fd, buf, count);
	if (r < 0) {
		perror("read");
		return 0;
	}

	return r;
}

int hidpp_open(void)
{
	int fd = -1;
	glob_t matches;
	char hiddev_name[32] = {0};

	if (!glob("/sys/class/hidraw/hidraw*/device/driver", 0, NULL, &matches)) {
		size_t i;
		char buf[1024];
		for (i = 0; i < matches.gl_pathc; i++) {
			ssize_t r;
			char *name = matches.gl_pathv[i];
			const char *last_comp;
			char *dev_name;

			r = readlink(name, buf, (sizeof buf) - 1);
			if (r < 0) {
				perror(name);
				continue;
			}

			buf[r] = 0; /* readlink does not NUL-terminate */
			last_comp = basename(buf);

			/* retrieve 'hidrawX' name */
			dev_name = name + sizeof "/sys/class/hidraw";
			*(strchr(dev_name, '/')) = 0;

			if (!strcmp(last_comp, RECEIVER_NAME)) {
				/* Logitech receiver c52b and c532 - pass */
			} else if (!strcmp(last_comp, "hid-generic")) {
				/* need to test for older nano receiver c52f */
				FILE *fp;
				uint32_t vid = 0, pid = 0;

				/* Assume that the first match is the receiver. Devices bound to
				 * the same receiver may have the same modalias. */
				snprintf(buf, sizeof buf, "/sys/class/hidraw/%s/device/modalias", dev_name);
				if ((fp = fopen(buf, "r"))) {
					int m = fscanf(fp, "hid:b%*04Xg%*04Xv%08Xp%08X", &vid, &pid);
					if (m != 2) {
						pid = 0;
					}
					fclose(fp);
				}

				if (vid != VID_LOGITECH ||
					(pid != PID_UNIFYING && pid != PID_NANO_RECEIVER)) {
					continue;
				}
			} else { /* unknown driver */
				continue;
			}

			snprintf(hiddev_name, sizeof hiddev_name, "/dev/%s", dev_name);
			fd = open(hiddev_name, O_RDWR);
			if (fd < 0) {
				perror(hiddev_name);
			} else {
				break;
			}
		}
	}

	if (fd < 0) {
		if (*hiddev_name) {
			fprintf(stderr, "Logitech Unifying Receiver device is not accessible.\n"
				"Try running this program as root or enable read/write permissions\n"
				"for %s\n", hiddev_name);
		} else {
			fprintf(stderr, "No Logitech Unifying Receiver device found\n");
			if (access("/sys/class/hidraw", R_OK)) {
				fputs("The kernel must have CONFIG_HIDRAW enabled.\n",
					stderr);
			}
			if (access("/sys/module/hid_logitech_dj", F_OK)) {
				fprintf(stderr, "Driver is not loaded, try:"
						"   sudo modprobe hid-logitech-dj\n");
			}
		}
	}
	globfree(&matches);

	return fd;
}

static void hidpp_discard_messages(int fd)
{
	char c;

	/* kernel discards remainder of message */
	while (poll_read(fd, 1, &c, 1)) ;
}

bool hidpp_write_report(int fd, struct HidppMessage *msg)
{
	ssize_t r, payload_size = HIDPP_SHORT_LEN;

	if (msg->report_id == HIDPP_LONG) {
		payload_size = HIDPP_LONG_LEN;
	}

	/* start with an empty queue to avoid interference */
	hidpp_discard_messages(fd);

	trace_dump_data("wr", (uint8_t *) msg, payload_size);

	r = write(fd, msg, payload_size);
	if (r < 0) {
		perror("write");
	}

	return payload_size == r;
}

bool hidpp_read_msg(int fd, int timeout, HidppMessage *msg,
                    hidpp_msg_cb_t *cb, void *userdata)
{
	int r;
	long long unsigned begin_ms = 0, end_ms;
	HidppMessage response;

	do {
		end_ms = get_timestamp_ms();
		if (begin_ms > 0)
			timeout -= end_ms - begin_ms;
		begin_ms = end_ms;
		if (timeout <= 0)
			break;

		memset(&response, 0, sizeof(response));
		r = poll_read(fd, timeout, &response, sizeof(response));
		if (r > 0) {
			trace_dump_data("rd", (uint8_t *) msg, r);

			if (r < HIDPP_SHORT_LEN) {
				trace_log("Impossible short read: %i\n", r);
			} else if (response.report_id == HIDPP_SHORT ||
			           response.report_id == HIDPP_LONG) {
				uint8_t ix = response.device_index;

				if (!is_valid_device_index(ix) && ix != 0xFF) {
					trace_log("Invalid device index: %i\n", ix);
				} else if (cb(&response, userdata)) {
					/* response is accepted, return message */
					memcpy(msg, &response, sizeof(response));
					return true;
				}
			}
		}
	} while (1);

	/* timeout expired, no report found unfortunately */
	return false;
}
