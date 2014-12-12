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
#include <stdlib.h>
#include "debug.h"
#include "hidpp.h"
#include "hidpp10.h"
#include "hidpp20.h"

int hidpp20_get_features_count(int fd, uint8_t device_index,
                               const FeatureInfo *ifeatureset,
                               uint8_t *count)
{
	int r;
	HidppMessage req = {
		.report_id      = HIDPP_SHORT,
		.device_index   = device_index,
		.feature_index  = ifeatureset->feature_index,
		.func           = HIDPP20_FUNC(0),
	};
	r = hidpp10_request(fd, &req, NULL, NULL);
	if (r == 0) {
		*count = req.params[0];
	} else {
		trace_log("[ix=%02x] IFeatureSet.GetCount failed with %#x\n",
			device_index, r);
	}
	return r;
}

int hidpp20_get_feature(int fd, uint8_t device_index,
                        const FeatureInfo *ifeatureset,
                        uint8_t feature_index, FeatureInfo *fi)
{
	int r;
	HidppMessage req = {
		.report_id      = HIDPP_SHORT,
		.device_index   = device_index,
		.feature_index  = ifeatureset->feature_index,
		.func           = HIDPP20_FUNC(1), /* GetFeatureId */
		.params         = { feature_index }
	};
	assert(device_index >= 1 && device_index <= MAX_DEVICES);

	r = hidpp10_request(fd, &req, NULL, NULL);
	if (r == 0) {
		fi->feature_index = feature_index;
		fi->feature_id = (req.params[0] << 8) | req.params[1];
		fi->feature_type = req.params[2];
	} else {
		trace_log("[ix=%02x] failed to get feature %i, error=%i\n",
			device_index, feature_index, r);
	}
	return r;
}

int hidpp20_get_feature_by_id(int fd, uint8_t device_index, uint16_t feature_id,
                              FeatureInfo *fi)
{
	int r;
	HidppMessage req = {
		.report_id      = HIDPP_SHORT,
		.device_index   = device_index,
		.feature_index  = 0x00, /* IRoot */
		.func           = HIDPP20_FUNC(0), /* GetFeature */
		.params         = { feature_id >> 8, (uint8_t) feature_id }
	};
	assert(device_index >= 1 && device_index <= MAX_DEVICES);

	r = hidpp10_request(fd, &req, NULL, NULL);
	if (r == 0) {
		fi->feature_index = req.params[0];
		fi->feature_id = feature_id;
		fi->feature_type = req.params[1];
	} else {
		trace_log("[ix=%02x] failed to get feature id %#06x, error=%i\n",
			device_index, feature_id, r);
	}
	return r;
}

FeatureInfo *hidpp20_get_features(int fd, uint8_t device_index, unsigned *count)
{
	int r, i;
	FeatureInfo fi;
	FeatureInfo *infos;
	uint8_t count_nonroot; /* features count not including root feature */

	assert(device_index >= 1 && device_index <= MAX_DEVICES);

	*count = 0;
	r = hidpp20_get_version(fd, device_index);
	if (r < 0x0200) {
		trace_log("[ix=%02x] HID++ 2.0 required for features, got %#4x\n",
			device_index, r);
		return NULL;
	}

	r = hidpp20_get_feature_by_id(fd, device_index, 0x0001, &fi);
	if (r) {
		trace_log("[ix=%02x] FeatureSet not found, error=%#x\n",
			device_index, r);
		return NULL;
	}
	r = hidpp20_get_features_count(fd, device_index, &fi, &count_nonroot);
	if (r)
		return NULL;
	infos = calloc(*count, sizeof(FeatureInfo));

	/* infos[0] is the Root feature which has index=0, id=0, type=0 */

	for (i = 1; i <= count_nonroot; i++) {
		r = hidpp20_get_feature(fd, device_index, &fi, i, &infos[i]);
		if (r) {
			trace_log("[ix=%02x] feature %i not found, error=%#x\n",
				device_index, i, r);
			free(infos);
			return NULL;
		}
	}
	/* maybe qsort(infos); here? */
	*count = count_nonroot + 1;
	return infos;
}

uint16_t hidpp20_get_version(int fd, uint8_t device_index)
{
	int r;
	uint16_t ver = 0;
	const uint8_t pingData = 0x14;
	HidppMessage req = {
		.report_id      = HIDPP_SHORT,
		.device_index   = device_index,
		.feature_index  = 0x00, /* IRoot */
		.func           = HIDPP20_FUNC(1),
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
