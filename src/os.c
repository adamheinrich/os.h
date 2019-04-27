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
	void (*handler)(void *p_params);
	void *p_params;
	volatile enum os_task_status status;
};

static enum {
	OS_STATE_DEFAULT = 1,
	OS_STATE_INITIALIZED,
	OS_STATE_TASKS_INITIALIZED,
	OS_STATE_STARTED,
} m_state = OS_STATE_DEFAULT;

static struct {
	struct os_task tasks[OS_CONFIG_MAX_TASKS];
	volatile uint32_t current_task;
	uint32_t size;
} m_task_table;

volatile struct os_task *os_curr_task;
volatile struct os_task *os_next_task;

void SysTick_Handler(void);

static void task_finished(void)
{
	/* This function is called when some task handler returns. */

	volatile uint32_t i = 0;
	while (1)
		i++;
}

bool os_init(void)
{
	if (m_state != OS_STATE_DEFAULT)
		return false;

	memset(&m_task_table, 0, sizeof(m_task_table));
	m_state = OS_STATE_INITIALIZED;

	return true;
}

bool os_task_init(void (*handler)(void *p_params), void *p_task_params,
		  uint32_t *p_stack, size_t stack_size)
{
	if (m_state != OS_STATE_INITIALIZED &&
	    m_state != OS_STATE_TASKS_INITIALIZED)
		return false;

	if (m_task_table.size >= OS_CONFIG_MAX_TASKS-1)
		return false;

	if ((stack_size % sizeof(uint32_t)) != 0) /* TODO: Use assert? */
		return false;

	uint32_t stack_offset = (stack_size/sizeof(uint32_t));

	/* Initialize the task structure and set SP to the top of the stack
	   minus 16 words (64 bytes) to leave space for storing 16 registers: */
	struct os_task *p_task = &m_task_table.tasks[m_task_table.size];
	p_task->handler = handler;
	p_task->p_params = p_task_params;
	p_task->sp = (uint32_t)(p_stack+stack_offset-16);
	p_task->status = OS_TASK_STATUS_IDLE;

	/* Save init. values of registers which will be restored on exc. return:
	   - XPSR: Default value (0x01000000)
	   - PC: Point to the handler function (with LSB masked because the
	         behavior is unpredictable if pc<0> == '1' on exc. return)
	   - LR: Point to a function to be called when the handler returns
	   - R0: Point to the handler function's parameter */
	p_stack[stack_offset-1] = 0x01000000;
	p_stack[stack_offset-2] = (uint32_t)handler & ~0x01UL;
	p_stack[stack_offset-3] = (uint32_t)&task_finished;
	p_stack[stack_offset-8] = (uint32_t)p_task_params;

#ifdef OS_CONFIG_DEBUG
	uint32_t base = (m_task_table.size+1)*1000;
	p_stack[stack_offset-4] = base+12;  /* R12 */
	p_stack[stack_offset-5] = base+3;   /* R3  */
	p_stack[stack_offset-6] = base+2;   /* R2  */
	p_stack[stack_offset-7] = base+1;   /* R1  */
	/* p_stack[stack_offset-8] is R0 */
	p_stack[stack_offset-9] = base+7;   /* R7  */
	p_stack[stack_offset-10] = base+6;  /* R6  */
	p_stack[stack_offset-11] = base+5;  /* R5  */
	p_stack[stack_offset-12] = base+4;  /* R4  */
	p_stack[stack_offset-13] = base+11; /* R11 */
	p_stack[stack_offset-14] = base+10; /* R10 */
	p_stack[stack_offset-15] = base+9;  /* R9  */
	p_stack[stack_offset-16] = base+8;  /* R8  */
#endif /* OS_CONFIG_DEBUG */

	m_state = OS_STATE_TASKS_INITIALIZED;
	m_task_table.size++;

	return true;
}

bool os_start(uint32_t systick_ticks)
{
	if (m_state != OS_STATE_TASKS_INITIALIZED)
		return false;

	NVIC_SetPriority(PendSV_IRQn, 0xff); /* Lowest possible priority */
	NVIC_SetPriority(SysTick_IRQn, 0x00); /* Highest possible priority */

	/* Start the SysTick timer: */
	uint32_t ret_val = SysTick_Config(systick_ticks);
	if (ret_val != 0)
		return false;

	/* Start the first task: */
	os_curr_task = &m_task_table.tasks[m_task_table.current_task];
	m_state = OS_STATE_STARTED;

	__set_PSP(os_curr_task->sp+64); /* Set PSP to the top of task's stack */
	__set_CONTROL(0x03); /* Switch to Unprivilleged Thread Mode with PSP */
	__ISB(); /* Execute ISB after changing CONTORL (recommended) */

	os_curr_task->handler(os_curr_task->p_params);

	return true;
}

void SysTick_Handler(void)
{
	os_curr_task = &m_task_table.tasks[m_task_table.current_task];
	os_curr_task->status = OS_TASK_STATUS_IDLE;

	/* Select next task: */
	m_task_table.current_task++;
	if (m_task_table.current_task >= m_task_table.size)
		m_task_table.current_task = 0;

	os_next_task = &m_task_table.tasks[m_task_table.current_task];
	os_next_task->status = OS_TASK_STATUS_ACTIVE;

	/* Trigger PendSV which performs the actual context switch: */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
