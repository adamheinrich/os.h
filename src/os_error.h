#ifndef OS_ERROR_H
#define OS_ERROR_H

#include "os_config.h"

typedef enum {
	OS_ERROR_OK = 1,
	OS_ERROR_WRONG_STATE,
	OS_ERROR_NO_MEM,
	OS_ERROR_INVALID_PARAM,
	OS_ERROR_TASK_FINISHED,
} os_error_t;

#ifdef OS_CONFIG_ERROR_HANDLER

void OS_CONFIG_ERROR_HANDLER(os_error_t err_code, const char *p_file, int line);

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