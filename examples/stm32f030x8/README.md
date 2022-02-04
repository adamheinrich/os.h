# STM32F030x8 Example

The example can be run on [STM32F030R8 Nucleo][1] board. It requires the
[STM32Cube][2] library which is included as a git submodule.

Flash the code using ``make flash`` and observe how an onboard LED connected to
``PA5`` changes frequency every second.

See [CM-Makefile][2] for ``Makefile`` configuration.

[1]: http://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-nucleo/nucleo-f030r8.html
[2]: http://www.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32cube-embedded-software.html?querycriteria=productId=LN1897
[3]: https://github.com/adamheinrich/cm-makefile
