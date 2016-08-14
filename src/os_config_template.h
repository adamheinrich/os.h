#ifndef OS_CONFIG_H
#define OS_CONFIG_H

/* Configure and include CMSIS library (or vendor's MCU header with CMSIS
   already configured and included): */
// #include <mcu_vendor_header.h>

/* The maximum number of tasks: */
#define OS_CONFIG_MAX_TASKS	10

/* Enable debugging: */
// #define OS_CONFIG_DEBUG

/* Enable error checking assertions (OS_ERROR_CHECK) by defining error handler
   function. See os_error.h for function prototype: */
//#define OS_CONFIG_ERROR_HANDLER	os_error_handler

#endif