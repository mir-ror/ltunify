/*
 * HID++ 2.0 protocol details.
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

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "debug.h"
#include "hidpp.h"
#include "hidpp10.h"
#include "hidpp20.h"

uint16_t hidpp20_get_version(int fd, uint8_t device_index)
{
	int r;
	uint16_t ver = 0;
	const uint8_t pingData = 0x14;
	HidppMessage req = {
		.report_id      = HIDPP_SHORT,
		.device_index   = device_index,
		.feature_id     = 0x00,
		.func           = 0x14,
		.params         = { 0, 0, pingData }
	};

	assert(device_index >= 1 && device_index <= MAX_DEVICES);

	r = hidpp10_request(fd, &req, NULL, NULL);
	if (r == 0 && req.params[2] == pingData) {
			ver = (req.params[0] << 8) | req.params[1];
	} else if (r == HIDPP_ERR_INVALID_SUBID) {
			ver = 0x0100;
	} else {
			trace_log("[ix=%02x] HID++ version unavailable, error=%i\n",
				device_index, r);
	}

	return ver;
}
