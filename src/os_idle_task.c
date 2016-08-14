#include "os_idle_task.h"

#define STACK_SIZE	32

static os_stack_t m_stack[STACK_SIZE];

static void idle_task_handler(void *p_args)
{
	volatile uint32_t i = 0;
	while (1)
		i++;
}

os_error_t os_idle_task_init(void)
{
	return os_task_init(&idle_task_handler, NULL, m_stack, STACK_SIZE);
}
