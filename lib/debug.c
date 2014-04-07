/*
 * Debugging facilities.
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

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

#ifdef ENABLE_TRACING
static bool debug_enabled;

void trace_log(const char *fmt, ...)
{
	va_list ap;
	if (debug_enabled) {
		vprintf(fmt, ap);
	}
	va_end(ap);
}

void trace_dump_data(const char *tag, const uint8_t *data, size_t len)
{
	if (!debug_enabled) {
		return;
	}

	/* do not mess with stderr colors */
	fflush(NULL);
	printf("\033[34m");
	printf("%s: ", tag);
	while (len-- > 0) {
		printf("%02x%c", *(data++),
		                  len == 0 ? '\n' : ' ');
	}
	printf("\033[m");
	fflush(NULL);
}
#endif
