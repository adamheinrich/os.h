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

#include <os.h>

enum os_task_status {
	OS_TASK_STATUS_IDLE = 1,
	OS_TASK_STATUS_ACTIVE,
};

struct os_task {
	/* The stack pointer (sp) has to be the first element as it is located
	   at the same address as the structure itself (which makes it possible
	   to locate it safely from assembly implementation of PendSV_Handler).
	   The compiler might add padding between other structure elements. */
	volatile uint32_t sp;
	void (*handler)(void *params);
	void *params;
	volatile enum os_task_status status;
};

static enum state {
	STATE_DEFAULT = 1,
	STATE_INITIALIZED,
	STATE_TASKS_INITIALIZED,
	STATE_STARTED,
} state = STATE_DEFAULT;

static struct task_table {
	struct os_task tasks[OS_CONFIG_MAX_TASKS];
	volatile size_t current_task;
	size_t size;
} task_table;

volatile struct os_task *os_curr_task;
volatile struct os_task *os_next_task;

void SysTick_Handler(void);

static void task_finished(void)
{
	/* This function is called when some task handler returns. */

	volatile unsigned int i = 0;
	while (1)
		i++;
}

bool os_init(void)
{
	if (state != STATE_DEFAULT)
		return false;

	memset(&task_table, 0, sizeof(task_table));
	state = STATE_INITIALIZED;

	return true;
}

bool os_task_init(void (*handler)(void *params), void *task_params,
		  uint32_t *stack, size_t stack_size)
{
	if (state != STATE_INITIALIZED &&
	    state != STATE_TASKS_INITIALIZED)
		return false;

	if (task_table.size >= OS_CONFIG_MAX_TASKS-1)
		return false;

	if ((stack_size % sizeof(uint32_t)) != 0) /* TODO: Use assert? */
		return false;

	uint32_t stack_offset = (stack_size/sizeof(uint32_t));

	/* Initialize the task structure and set SP to the top of the stack
	   minus 16 words (64 bytes) to leave space for storing 16 registers: */
	struct os_task *task = &task_table.tasks[task_table.size];
	task->handler = handler;
	task->params = task_params;
	task->sp = (uint32_t)(stack+stack_offset-16);
	task->status = OS_TASK_STATUS_IDLE;

	/* Save init. values of registers which will be restored on exc. return:
	   - XPSR: Default value (0x01000000)
	   - PC: Point to the handler function (with LSB masked because the
	         behavior is unpredictable if pc<0> == '1' on exc. return)
	   - LR: Point to a function to be called when the handler returns
	   - R0: Point to the handler function's parameter */
	stack[stack_offset-1] = 0x01000000;
	stack[stack_offset-2] = (uint32_t)handler & ~0x01UL;
	stack[stack_offset-3] = (uint32_t)&task_finished;
	stack[stack_offset-8] = (uint32_t)task_params;

#ifdef OS_CONFIG_DEBUG
	size_t base = (task_table.size+1)*1000;
	stack[stack_offset-4] = base+12;  /* R12 */
	stack[stack_offset-5] = base+3;   /* R3  */
	stack[stack_offset-6] = base+2;   /* R2  */
	stack[stack_offset-7] = base+1;   /* R1  */
	/* stack[stack_offset-8] is R0 */
	stack[stack_offset-9] = base+7;   /* R7  */
	stack[stack_offset-10] = base+6;  /* R6  */
	stack[stack_offset-11] = base+5;  /* R5  */
	stack[stack_offset-12] = base+4;  /* R4  */
	stack[stack_offset-13] = base+11; /* R11 */
	stack[stack_offset-14] = base+10; /* R10 */
	stack[stack_offset-15] = base+9;  /* R9  */
	stack[stack_offset-16] = base+8;  /* R8  */
#endif /* OS_CONFIG_DEBUG */

	state = STATE_TASKS_INITIALIZED;
	task_table.size++;

	return true;
}

bool os_start(uint32_t systick_ticks)
{
	if (state != STATE_TASKS_INITIALIZED)
		return false;

	NVIC_SetPriority(PendSV_IRQn, 0xff); /* Lowest possible priority */
	NVIC_SetPriority(SysTick_IRQn, 0x00); /* Highest possible priority */

	/* Start the SysTick timer: */
	uint32_t ret_val = SysTick_Config(systick_ticks);
	if (ret_val != 0)
		return false;

	/* Start the first task: */
	os_curr_task = &task_table.tasks[task_table.current_task];
	state = STATE_STARTED;

	__set_PSP(os_curr_task->sp+64); /* Set PSP to the top of task's stack */
	__set_CONTROL(0x03); /* Switch to Unprivilleged Thread Mode with PSP */
	__ISB(); /* Execute ISB after changing CONTORL (recommended) */

	os_curr_task->handler(os_curr_task->params);

	return true;
}

void SysTick_Handler(void)
{
	os_curr_task = &task_table.tasks[task_table.current_task];
	os_curr_task->status = OS_TASK_STATUS_IDLE;

	/* Select next task: */
	task_table.current_task++;
	if (task_table.current_task >= task_table.size)
		task_table.current_task = 0;

	os_next_task = &task_table.tasks[task_table.current_task];
	os_next_task->status = OS_TASK_STATUS_ACTIVE;

	/* Trigger PendSV which performs the actual context switch: */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
