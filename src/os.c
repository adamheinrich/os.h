#include "os.h"

typedef enum {
	OS_TASK_STATUS_IDLE = 1,
	OS_TASK_STATUS_ACTIVE,
} os_task_status_t;

typedef struct {
	/* The stack pointer (sp) has to be the first element as it is located
	   at the same address as the structure itself (which makes it possible
	   to locate it safely from assembly implementation of PendSV_Handler).
	   The compiler might add padding between other structure elements. */
	volatile uint32_t sp;
	void (*handler)(void);
	volatile os_task_status_t status;
} os_task_t;

static struct {
	os_task_t tasks[OS_CONFIG_MAX_TASKS];
	volatile uint32_t current_task;
	uint32_t size;
} m_task_table;

volatile os_task_t *os_curr_task;
volatile os_task_t *os_next_task;

static void task_finished(void)
{
	/* This function is called when some task handler returns. */
	volatile uint32_t i = 0;
	while (1)
		i++;
}

void os_init(void)
{
	memset(&m_task_table, 0, sizeof(m_task_table));
}

bool os_task_init(void (*handler)(void), os_stack_t *p_stack, uint32_t stack_size)
{
	if (m_task_table.size >= OS_CONFIG_MAX_TASKS-1)
		return false;

	/* Initialize the task structure and set SP to the top of the stack
	   minus 16 words (64 bytes) to leave space for storing 16 registers: */
	os_task_t *p_task = &m_task_table.tasks[m_task_table.size];
	p_task->handler = handler;
	p_task->sp = (uint32_t)(p_stack+stack_size-16);
	p_task->status = OS_TASK_STATUS_IDLE;

	/* Save special registers which will be restored on exc. return:
	   - XPSR: Default value (0x01000000)
	   - PC: Point to the handler function
	   - LR: Point to a function to be called when the handler returns */
	p_stack[stack_size-1] = 0x01000000;
	p_stack[stack_size-2] = (uint32_t)handler;
	p_stack[stack_size-3] = (uint32_t) &task_finished;

#ifdef OS_CONFIG_DEBUG
	uint32_t base = (m_task_table.size+1)*1000;
	p_stack[stack_size-4] = base+12;  /* R12 */
	p_stack[stack_size-5] = base+3;   /* R3  */
	p_stack[stack_size-6] = base+2;   /* R2  */
	p_stack[stack_size-7] = base+1;   /* R1  */
	p_stack[stack_size-8] = base+0;   /* R0  */
	p_stack[stack_size-9] = base+7;   /* R7  */
	p_stack[stack_size-10] = base+6;  /* R6  */
	p_stack[stack_size-11] = base+5;  /* R5  */
	p_stack[stack_size-12] = base+4;  /* R4  */
	p_stack[stack_size-13] = base+11; /* R11 */
	p_stack[stack_size-14] = base+10; /* R10 */
	p_stack[stack_size-15] = base+9;  /* R9  */
	p_stack[stack_size-16] = base+8;  /* R8  */
#endif

	m_task_table.size++;

	return true;
}

bool os_start(uint32_t systick_ticks)
{
	NVIC_SetPriority(PendSV_IRQn, 0xff); /* Lowest possible priority */
	NVIC_SetPriority(SysTick_IRQn, 0x00); /* Highest possible priority */

	/* Start the SysTick timer: */
	uint32_t ret_val = SysTick_Config(systick_ticks);
	if (ret_val != 0)
		return false;

	/* Start the first task: */
	os_curr_task = &m_task_table.tasks[m_task_table.current_task];

	__set_PSP(os_curr_task->sp+64); /* Set PSP to the top of task's stack */
	__set_CONTROL(0x03); /* Switch to PSP, unprivilleged mode */
	__ISB(); /* Exec. ISB after changing CONTORL (recommended) */

	os_curr_task->handler();

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
