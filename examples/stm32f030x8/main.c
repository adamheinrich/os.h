#include <stm32f0xx.h>
#include "os.h"

#define TASK_STACK_SIZE		128

#define LED_GPIO_ENABLE_CLK()	RCC->AHBENR |= RCC_AHBENR_GPIOAEN
#define LED_GPIOx		GPIOA

#define LED1_GPIO_PIN		5
#define LED2_GPIO_PIN		6
#define LED3_GPIO_PIN		7
#define LED4_GPIO_PIN		8

#define MS_TO_TICKS(ms)		(ms * (SystemCoreClock / 1000))

typedef struct {
	uint32_t gpio_pin;
	uint32_t delay_ms;
} task_params_t;

static void task_handler(void *p_params);

void HardFault_Handler(void)
{
	while (1);
}

void os_error_handler(os_error_t error_code, const char *p_file, int line)
{
	while (1);
}

int main(void)
{
	os_error_t err_code;

	/* Initialize task stacks: */
	static os_stack_t stack1[TASK_STACK_SIZE];
	static os_stack_t stack2[TASK_STACK_SIZE];
	static os_stack_t stack3[TASK_STACK_SIZE];
	static os_stack_t stack4[TASK_STACK_SIZE];

	/* Initialize task parameters: */
	task_params_t params1 = {
		.gpio_pin = LED1_GPIO_PIN,
		.delay_ms = 50,
	};

	task_params_t params2 = {
		.gpio_pin = LED2_GPIO_PIN,
		.delay_ms = 200,
	};

	task_params_t params3 = {
		.gpio_pin = LED3_GPIO_PIN,
		.delay_ms = 333,
	};

	task_params_t params4 = {
		.gpio_pin = LED4_GPIO_PIN,
		.delay_ms = 500,
	};

	/* Initialize OS: */
	err_code = os_init();
	OS_ERROR_CHECK(err_code);

	err_code = os_task_init(&task_handler, (void *)&params1, stack1, TASK_STACK_SIZE);
	OS_ERROR_CHECK(err_code);
	err_code = os_task_init(&task_handler, (void *)&params2, stack2, TASK_STACK_SIZE);
	OS_ERROR_CHECK(err_code);
	err_code = os_task_init(&task_handler, (void *)&params3, stack3, TASK_STACK_SIZE);
	OS_ERROR_CHECK(err_code);
	err_code = os_task_init(&task_handler, (void *)&params4, stack4, TASK_STACK_SIZE);
	OS_ERROR_CHECK(err_code);

	/* Context switch every millisecond: */
	err_code = os_start(MS_TO_TICKS(1));
	OS_ERROR_CHECK(err_code);

	/* The program should never reach here: */
	while (1);
}

static void task_handler(void *p_params)
{
	uint32_t gpio_pin = ((task_params_t *) p_params)->gpio_pin;
	uint32_t delay_ms = ((task_params_t *) p_params)->delay_ms;

	__disable_irq();
	LED_GPIO_ENABLE_CLK();
	LED_GPIOx->MODER |= (1U << (2*gpio_pin));
	__enable_irq();

	while (1) {
		__disable_irq();
		LED_GPIOx->ODR ^= (1U << gpio_pin);
		__enable_irq();

		os_delay(delay_ms);
	}
}
