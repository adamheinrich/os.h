#ifndef OS_H
#define OS_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "os_config.h"
#include "os_error.h"

typedef uint32_t os_stack_t;

os_error_t os_init(void);
os_error_t os_task_init(void (*handler)(void *p_params), void *p_task_params,
			os_stack_t *p_stack, size_t stack_size);
os_error_t os_start(uint32_t systick_ticks);

#endif