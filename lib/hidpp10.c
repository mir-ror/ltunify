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

#include <stdbool.h>
#include <string.h>
#include "hidpp10.h"
#include "hidpp.h"

#define READ_TIMEOUT 2000

static bool hidpp10_msg_filter(HidppMessage *msg, void *userdata)
{
	bool ok;
	struct {
		hidpp_msg_cb_t *cb;
		void *userdata;
		HidppMessage *msg_out;
	} *cd = userdata;
	const HidppMessage *out = cd->msg_out;
	const uint8_t out_addr = out->address;

	ok = msg->device_index == out->device_index;

	/* accept error responses matching the request, return as there are no
	 * notifications to check for. */
	if (ok && msg->report_id == HIDPP_SHORT && msg->sub_id == SUB_ERROR_MSG
		&& msg->sub_id == out->sub_id && msg->params[0] == out_addr) {
		return true;
	}

	ok = ok && msg->sub_id == out->sub_id && msg->address == out_addr;
	/* Non-HID++ messages handling, maybe the callback is interested. */
	if (msg->report_id != HIDPP_SHORT && msg->report_id != HIDPP_LONG) {
		if (cd->cb) {
			cd->cb(msg, cd->userdata);
		}
		return false;
	}

	/* HID++ report handling */
	switch (msg->sub_id) {
	case SUB_SET_REGISTER:
	case SUB_GET_REGISTER:
	case SUB_SET_LONG_REGISTER:
		return ok && msg->report_id == HIDPP_SHORT;
	case SUB_GET_LONG_REGISTER:
		return ok && msg->report_id == HIDPP_LONG;
	case NOTIF_DEV_DISCONNECT:
	case NOTIF_DEV_CONNECT:
	case NOTIF_RECV_LOCK_CHANGE:
		if (cd->cb) {
			/* notify of ... well... notifications */
			cd->cb(msg, cd->userdata);
		}
		/* fall-through */
	default:
		/* no idea whether the report IDs must match or not */
		return ok;
	}
}

int hidpp10_request(int fd, HidppMessage *msg, hidpp_msg_cb_t *cb, void *userdata)
{
	HidppMessage msg_in;
	struct {
		hidpp_msg_cb_t *cb;
		void *userdata;
		HidppMessage *msg_out;
	} cd = { cb, userdata, msg };

	if (!hidpp_write_report(fd, msg))
		return -1;

	/* find the HID++ response, passing notifications if needed */
	if (!hidpp_read_msg(fd, READ_TIMEOUT, &msg_in, hidpp10_msg_filter, &cd))
		return -1;

	memcpy(msg, &msg_in, sizeof(msg_in));
	return msg_in.sub_id == SUB_ERROR_MSG ? msg_in.params[1] : 0;
}

int hidpp10_enable_wireless_notifications(int fd, bool enabled)
{
	int r;
	HidppMessage msg = {
		.report_id = HIDPP_SHORT,
		.device_index = 0xFF,
		.sub_id = SUB_GET_REGISTER,
		.address = REG_ENABLED_NOTIFS
	};

	/* get notifications state */
	r = hidpp10_request(fd, &msg, NULL, NULL);
	if (r == 0 && !(msg.params[1] & 1) != !enabled) {
		msg.params[1] |= enabled ? 1 : 0;

		r = hidpp10_request(fd, &msg, NULL, NULL);
	}
	return r;
}
