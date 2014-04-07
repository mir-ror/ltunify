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

#ifndef HIDPP_H
#define HIDPP_H
#include <stdbool.h>
#include <stdint.h>

#define MAX_DEVICES 6

/* report ID for short and long HID++ messages */
#define HIDPP_SHORT             0x10
#define HIDPP_SHORT_LEN         7
#define HIDPP_LONG              0x11
#define HIDPP_LONG_LEN          20

typedef struct HidppMessage {
	uint8_t report_id;
	uint8_t device_index;
	union {
		struct {
			uint8_t sub_id;
			uint8_t address;
		}; /**< HID++ 1.0 naming */
		struct {
			uint8_t feature_id;
			uint8_t func; /* (func << 4) | swId */
		}; /**< HID++ 2.0 naming */
	};
	union {
		uint8_t params[3];
		uint8_t params_l[16];
	};
} HidppMessage;

/**
 * Locates a /dev/hidrawX device for a Logitech Unifying receiver.
 *
 * @return A file descriptor to the hidraw device which must be closed with
 * close().
 */
int hidpp_open(void);

/**
 * Callback function for new messages.
 *
 * @param msg[in]   An incoming HID++ message.
 * @param userdata  Data passed through the invoker.
 * @return true iff the no more messages should be read.
 */
typedef bool hidpp_msg_cb_t(HidppMessage *msg, void *userdata);

/**
 * Sends the message to the receiver device.
 *
 * @param fd        File descriptor of the hidraw device.
 * @param msg[in]   The report to be delivered.
 * @return true iff the message is properly formatted and the dispatch is
 * successful.
 */
bool hidpp_write_report(int fd, struct HidppMessage *msg);

/**
 * Reads messages and try to accept on one of them.
 *
 * @param fd        File descriptor of the hidraw device.
 * @param timeout   Timeout for this function in milliseconds.
 * @param msg[out]  On success, the message is written to this pointer.
 *                  Otherwise, the contents are unmodified.
 * @param cb        Function that should be called for new HID++ messages.
 *                  If true is returned, then no more messages are read. msg is
 *                  not allowed to be modified if the callback returns false.
 * @param userdata  Data that is passed unchanged to the callback function.
 * @return true iff there exists a message that was accepted by the callback
 * function within the timeout.
 */
bool hidpp_read_msg(int fd, int timeout, HidppMessage *msg,
                    hidpp_msg_cb_t *cb, void *userdata);
#endif
