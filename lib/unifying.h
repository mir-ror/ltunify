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

#ifndef UNIFYING_H
#define UNIFYING_H
#include <stdint.h>

typedef struct UnifyingState UnifyingState;

typedef enum {
	FWVER_MAIN = 0, /**< Main firmware version */
	FWVER_BL,       /**< Bootloader version */
	FWVER_HW,       /**< Hardware version */
} FirmwareType;

typedef struct {
	char prefix[4]; /**< Prefix of the firmware version (if any) */
	uint8_t major;
	uint8_t minor;
	uint16_t build;
} FirmwareVersion;

/**
 * Attempts to open a Unifying device.
 *
 * @param fd    File descriptor of the hidraw device.
 * @return A pointer to a structure that must be released by
 *         unifying_device_close(), or NULL on error.
 */
UnifyingState *unifying_new(int fd);

/**
 * Closes the file descriptor associated with this device and releases memory
 * claimed for this structure.
 *
 * @param s     State to be freed.
 */
void unifying_close(UnifyingState *s);

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
