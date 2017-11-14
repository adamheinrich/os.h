/*
 * This file is part of os.h.
 *
 * Copyright (C) 2016 Adam Heinrich <adam@adamh.cz>
 *
 * Os.h is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Os.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with os.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OS_CONFIG_H
#define OS_CONFIG_H

/* Configure and include CMSIS library (or vendor's MCU header with CMSIS
   already configured and included): */
// #include <mcu_vendor_header.h>

/* The maximum number of tasks: */
#define OS_CONFIG_MAX_TASKS	10

/* Enable debugging: */
// #define OS_CONFIG_DEBUG

#endif /* OS_CONFIG_H */
