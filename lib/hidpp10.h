/*
 * HID++ 1.0 protocol details.
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

#ifndef HIDPP_10_H
#define HIDPP_10_H
#include <stdint.h>
#include "hidpp.h"

#define SUB_SET_REGISTER        0x80
#define SUB_GET_REGISTER        0x81
#define SUB_SET_LONG_REGISTER   0x82
#define SUB_GET_LONG_REGISTER   0x83
#define SUB_ERROR_MSG           0x8F

#define NOTIF_DEV_DISCONNECT    0x40 /* Device Disconnection */
#define NOTIF_DEV_CONNECT       0x41 /* Device Connection */
#define NOTIF_RECV_LOCK_CHANGE  0x4A /* Unifying Receiver Locking Change information */

#define REG_ENABLED_NOTIFS      0x00
#define REG_CONNECTION_STATE    0x02
/* Device Connection and Disconnection (Pairing) */
#define REG_DEVICE_PAIRING      0xB2
#define REG_DEVICE_ACTIVITY     0xB3
#define REG_PAIRING_INFO        0xB5
#define REG_VERSION_INFO        0xF1 /* undocumented */

/* HID++ 1.0 error codes */
#define HIDPP_ERR_SUCCESS               0x00
#define HIDPP_ERR_INVALID_SUBID         0x01
#define HIDPP_ERR_INVALID_ADDRESS       0x02
#define HIDPP_ERR_INVALID_VALUE         0x03
#define HIDPP_ERR_CONNECT_FAIL          0x04
#define HIDPP_ERR_TOO_MANY_DEVICES      0x05
#define HIDPP_ERR_ALREADY_EXISTS        0x06
#define HIDPP_ERR_BUSY                  0x07
#define HIDPP_ERR_UNKNOWN_DEVICE        0x08
#define HIDPP_ERR_RESOURCE_ERROR        0x09
#define HIDPP_ERR_REQUEST_UNAVAILABLE   0x0A
#define HIDPP_ERR_INVALID_PARAM_VALUE   0x0B
#define HIDPP_ERR_WRONG_PIN_CODE        0x0C

/**
 * Attempts to dispatch a register query.
 *
 * @param fd        File descriptor of the hidraw device.
 * @param msg[in,out] The message to be sent. If a matching response was
 *                  received, then this will contain that response. Otherwise,
 *                  it is unmodified.
 * @param cb        Function that should be called for HID++ notifications. Its
 *                  return value is ignored. The callback must not modify msg.
 * @return 0 on success, a HID++ 1.0 error code otherwise.
 */
int hidpp10_request(int fd, HidppMessage *msg, hidpp_msg_cb_t *cb, void *userdata);

/**
 * Attempts to enable/disable reporting of wireless notifications.
 *
 * @param fd        File descriptor of the hidraw device.
 * @param enabled   Whether wireless notifications should be reported.
 * @return 0 on success, a HID++ 1.0 error code otherwise.
 */
int hidpp10_enable_wireless_notifications(int fd, bool enabled);
#endif
