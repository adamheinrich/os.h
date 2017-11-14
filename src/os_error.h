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

#ifndef OS_ERROR_H
#define OS_ERROR_H

#include "os_config.h"

enum os_error {
	OS_ERROR_OK = 1,
	OS_ERROR_WRONG_STATE,
	OS_ERROR_NO_MEM,
	OS_ERROR_INVALID_PARAM,
	OS_ERROR_TASK_FINISHED,
};

#ifdef OS_CONFIG_ERROR_HANDLER

void OS_CONFIG_ERROR_HANDLER(enum os_error err_code, const char *p_file,
			     int line);

#define OS_ERROR_HANDLER(err_code) \
	OS_CONFIG_ERROR_HANDLER(err_code, __FILE__, __LINE__)

#define OS_ERROR_CHECK(err_code) \
	do { \
		if (err_code != OS_ERROR_OK) \
			OS_ERROR_HANDLER(err_code); \
	} while (0)

#else

#define OS_ERROR_HANDLER(err_code)	(void)(err_code)
#define OS_ERROR_CHECK(err_code)	(void)(err_code)

#endif

#endif