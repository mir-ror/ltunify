/*
 * Generic routines for communicating with Logitech Unifying devices.
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

#include <stdlib.h>
#include <unistd.h>
#include "unifying.h"
#include "hidpp.h"
#include "hidpp20.h"

typedef struct HidppDevice {
	uint8_t     device_index;
	uint16_t    hidpp_version;
	uint16_t    wireless_pid;
	uint8_t     device_type;
	uint32_t    serial_number;
	char        name[15];       /**< Short UTF-8 encoded name string. */
	uint16_t    notification_flags;
	union {
#if 0
		struct {
		}; /**< details specific to HID++ 1.0 */
#endif
		struct {
			unsigned features_count;
			FeatureInfo *features;
		}; /**< details specific to HID++ 2.0 */
	};
} HidppDevice;

struct UnifyingState {
	int fd;
	uint8_t devices_count; /**< The number of paired devices */
	/**
	 * Devices that are currently paired to the receiver. If the device_index
	 * property of a device is zero, then either the device is not paired or the
	 * device information has not been acquired yet.
	 */
	HidppDevice devices[MAX_DEVICES];
	struct {
		uint8_t     notification_flags;
		uint32_t    serial_number;
	} receiver;
};

UnifyingState *unifying_new(int fd)
{
	UnifyingState *s;

	if (fd < 0)
		return NULL;

	s = calloc(1, sizeof(UnifyingState));
	if (!s)
		return NULL;

	s->fd = fd;

	return s;
}

void unifying_close(UnifyingState *s)
{
	if (s->fd >= 0)
		close(s->fd);

	free(s);
}

#if 0
int unifying_rvr_get_version(UnifyingState *s, FirmwareType type,
                             FirmwareVersion *fw);
int unifying_rvr_get_serial(UnifyingState *s, uint32_t *serial);

/* general device information */
int unifying_dev_get_version(UnifyingState *s, uint8_t ix, FirmwareType type,
                             FirmwareVersion *fw);
int unifying_dev_get_short_name(UnifyingState *s, uint8_t ix, char *name);
int unifying_dev_get_name(UnifyingState *s, uint8_t ix, char **name);
int unifying_dev_get_wpid(UnifyingState *s, uint8_t ix, uint16_t *wpid);
int unifying_dev_get_type(UnifyingState *s, uint8_t ix, uint8_t *devtype);
int unifying_dev_get_serial(UnifyingState *s, uint8_t ix, uint32_t *serial);

/* pairing related */
int unifying_dev_pairing_open(UnifyingState *s);
int unifying_dev_pairing_unpair(UnifyingState *s, uint8_t ix);
int unifying_dev_pairing_close(UnifyingState *s);
#endif
