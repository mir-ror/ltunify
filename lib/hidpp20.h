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

#ifndef HIDPP_20_H
#define HIDPP_20_H
#include <stdint.h>
#include "hidpp.h"

typedef struct {
	uint8_t     feature_index;
	uint16_t    feature_id;
	uint8_t     feature_type;
} FeatureInfo;

/**
 * Retrieves the number of HID++ 2.0 features.
 *
 * @param fd        File descriptor of the hidraw device.
 * @param count[out] The number of HID++ 2.0 features.
 * @return Zero on success, a HID++ error code otherwise.
 */
int hidpp20_get_features_count(int fd, uint8_t *count);

/**
 * Retrieves feature information from a feature index.
 *
 * @param fd        File descriptor of the hidraw device.
 * @param feature_index A feature index to find information for.
 * @param fi[out]   Feature information on success.
 * @return Zero on success, a HID++ error code otherwise.
 */
int hidpp20_get_feature(int fd, uint8_t feature_index, FeatureInfo *fi);

/**
 * Retrieves the feature index for a given feature ID.
 *
 * @param fd        File descriptor of the hidraw device.
 * @param feature_id A 16-bit feature identifier.
 * @param fi[out]   Feature information on success.
 * @return Zero on success, a HID++ error code otherwise.
 */
int hidpp20_get_feature_by_id(int fd, uint16_t feature_id, FeatureInfo *fi);

/**
 * Retrieves the HID++ version for the device.
 *
 * @param fd        File descriptor of the hidraw device.
 * @param device_index Device index (between 1 and 6).
 * @return a non-zero number indicating the HID++ version or 0 if the version
 * could not be determined.
 */
uint16_t hidpp20_get_version(int fd, uint8_t device_index);
#endif
