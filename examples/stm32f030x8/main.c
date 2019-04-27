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

#include <stm32f0xx.h>
#include <os.h>

#define LED_GPIO_ENABLE_CLK()	RCC->AHBENR |= RCC_AHBENR_GPIOAEN
#define LED_GPIOx		GPIOA
#define LED_GPIO_PIN		5

#define ERROR_CHECK(status) \
	do { \
		if (!(status)) \
			while (1); \
	} while (0)

void HardFault_Handler(void);
static void delay(volatile uint32_t delay_ms);
static void task_handler(void *params);

void HardFault_Handler(void)
{
	while (1);
}

int main(void)
{
	bool status;

	/* Enable clock for GPIOA and set GPIO pin PA5 as output: */
	LED_GPIO_ENABLE_CLK();
	LED_GPIOx->MODER |= (1U << (2*LED_GPIO_PIN));

	/* Initialize task stacks: */
	static uint32_t stack1[128];
	static uint32_t stack2[128];
	static uint32_t stack3[128];

	/* Setup task parameters: */
	uint32_t p1 = 200000;
	uint32_t p2 = p1/2;
	uint32_t p3 = p1/4;

	status = os_init();
	ERROR_CHECK(status);

	status = os_task_init(&task_handler, (void*)p1, stack1, sizeof(stack1));
	ERROR_CHECK(status);
	status = os_task_init(&task_handler, (void*)p2, stack2, sizeof(stack2));
	ERROR_CHECK(status);
	status = os_task_init(&task_handler, (void*)p3, stack3, sizeof(stack3));
	ERROR_CHECK(status);

	/* Context switch every second: */
	status = os_start(SystemCoreClock);
	ERROR_CHECK(status);

	/* The program should never reach here: */
	while (1);
}

static void delay(volatile uint32_t time)
{
	while (time > 0)
		time--;
}

static void task_handler(void *params)
{
	uint32_t delay_time = (uint32_t)params;

	while (1) {
		__disable_irq();
		LED_GPIOx->ODR ^= (1U << LED_GPIO_PIN);
		__enable_irq();

		delay(delay_time);
	}
}
