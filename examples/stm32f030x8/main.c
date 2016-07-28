#include <stm32f0xx.h>
#include "os.h"

#define LED_GPIO_ENABLE_CLK()	RCC->AHBENR |= RCC_AHBENR_GPIOAEN
#define LED_GPIOx		GPIOA
#define LED_GPIO_PIN		5

static void delay(volatile uint32_t delay_ms);
static void task1_handler(void);
static void task2_handler(void);
static void task3_handler(void);

void HardFault_Handler(void)
{
	while(1);
}

int main(void)
{
	/* Enable clock for GPIOA and set GPIO pin PA5 as output: */
	LED_GPIO_ENABLE_CLK();
	LED_GPIOx->MODER |= (1U << (2*LED_GPIO_PIN));

	/* Initialize task stacks: */
	static os_stack_t stack1[128];
	static os_stack_t stack2[128];
	static os_stack_t stack3[128];

	os_init();

	os_task_init(&task1_handler, stack1, 128);
	os_task_init(&task2_handler, stack2, 128);
	os_task_init(&task3_handler, stack3, 128);
	
	/* Context switch every second: */
	os_start(SystemCoreClock);

	/* The program should never reach there: */
	while (1);
}

static void delay(volatile uint32_t time)
{
	while (time > 0)
		time--;
}

static void task1_handler(void)
{
	while (1) {
		__disable_irq();
		LED_GPIOx->ODR ^= (1U << LED_GPIO_PIN);
		__enable_irq();

		delay(100000);
	}
}

static void task2_handler(void)
{
	while (1) {
		__disable_irq();
		LED_GPIOx->ODR ^= (1U << LED_GPIO_PIN);
		__enable_irq();

		delay(50000);
	}
}

static void task3_handler(void)
{
	while (1) {
		__disable_irq();
		LED_GPIOx->ODR ^= (1U << LED_GPIO_PIN);
		__enable_irq();

		delay(10000);
	}
}
