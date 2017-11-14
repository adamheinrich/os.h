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

/* Include CMSIS library: */
#include <stm32f0xx.h>

/* The maximum number of tasks: */
#define OS_CONFIG_MAX_TASKS	10

/* Enable debugging: */
#define OS_CONFIG_DEBUG

/* Enable error checking assertions (OS_ERROR_CHECK) by defining error handler
   function. See os_error.h for function prototype: */
#define OS_CONFIG_ERROR_HANDLER	os_error_handler

#endif /* OS_CONFIG_H */
