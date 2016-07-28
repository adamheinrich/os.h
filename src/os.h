#ifndef OS_H
#define OS_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "os_config.h"

typedef uint32_t os_stack_t;

void os_init(void);
bool os_task_init(void (*handler)(void), os_stack_t *p_stack, uint32_t stack_size);
bool os_start(uint32_t systick_ticks);

#endif