#include <stm32f0xx.h>
#include <os.h>

#define LED_GPIO_ENABLE_CLK()	RCC->AHBENR |= RCC_AHBENR_GPIOAEN
#define LED_GPIOx		GPIOA
#define LED_GPIO_PIN		5

void HardFault_Handler(void);
static void delay(volatile uint32_t delay_ms);
static void task_handler(void *p_params);

void HardFault_Handler(void)
{
	while (1);
}

void os_error_handler(os_error_t error_code, const char *p_file, int line)
{
	(void)error_code;
	(void)p_file;
	(void)line;

	while (1);
}

int main(void)
{
	os_error_t err_code;

	/* Enable clock for GPIOA and set GPIO pin PA5 as output: */
	LED_GPIO_ENABLE_CLK();
	LED_GPIOx->MODER |= (1U << (2*LED_GPIO_PIN));

	/* Initialize task stacks: */
	static os_stack_t stack1[128];
	static os_stack_t stack2[128];
	static os_stack_t stack3[128];

	err_code = os_init();
	OS_ERROR_CHECK(err_code);

	err_code = os_task_init(&task_handler, (void *)100000, stack1, 128);
	OS_ERROR_CHECK(err_code);
	err_code = os_task_init(&task_handler, (void *)50000, stack2, 128);
	OS_ERROR_CHECK(err_code);
	err_code = os_task_init(&task_handler, (void *)10000, stack3, 128);
	OS_ERROR_CHECK(err_code);

	/* Context switch every second: */
	err_code = os_start(SystemCoreClock);
	OS_ERROR_CHECK(err_code);

	/* The program should never reach there: */
	while (1);
}

static void delay(volatile uint32_t time)
{
	while (time > 0)
		time--;
}

static void task_handler(void *p_params)
{
	uint32_t delay_time = (uint32_t)p_params;

	while (1) {
		__disable_irq();
		LED_GPIOx->ODR ^= (1U << LED_GPIO_PIN);
		__enable_irq();

		delay(delay_time);
	}
}
