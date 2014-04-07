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

#ifndef DEBUG_H
#define DEBUG_H
#include <stdint.h>

#ifdef ENABLE_TRACING
void trace_dump_data(const char *tag, const uint8_t *data, size_t len);

void trace_log(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
#else
#define U __attribute__((__unused__))
static inline void trace_dump_data(const char U *tag, const uint8_t U *data, size_t U len) { }

static inline void trace_log(const char U *fmt, ...) {}
#undef U
#endif
#endif
