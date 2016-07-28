# STM32F030x8 Example

The example can be run on [STM32F030R8 Nucleo][1] board. It requires the
STM32Cube library (see README in the ``../lib`` directory).

Flash the code using ``make flash`` and observe how an onboard LED connected to
``PA5`` changes frequency every second.

See [stm32-template][2] for ``Makefile`` configuration.

[1]: http://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-nucleo/nucleo-f030r8.html
[2]: https://github.com/adamheinrich/stm32-template
