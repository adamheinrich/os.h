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

#endif