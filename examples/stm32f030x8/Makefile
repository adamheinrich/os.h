##
##  This file is part of os.h.
##
##  Copyright (C) 2016 Adam Heinrich <adam@adamh.cz>
##
##  Os.h is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  Os.h is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with os.h.  If not, see <http://www.gnu.org/licenses/>.
##

# Project name
BIN = os-example

# Include paths (STM32CubeF0 SDK)
OS_DIR = ../../src
CUBE_DIR = STM32Cube_FW_F0_V1.4.0
CUBE_DEVICE_DIR = $(CUBE_DIR)/Drivers/CMSIS/Device/ST/STM32F0xx

INC = -I$(CUBE_DIR)/Drivers/CMSIS/Include \
      -I$(CUBE_DEVICE_DIR)/Include \
      -I$(OS_DIR)

# ASM and C (e.g. startup) source files and linker script outside src directory
SRC_ASM = $(CUBE_DEVICE_DIR)/Source/Templates/gcc/startup_stm32f030x8.s \
          $(OS_DIR)/os_pendsv_handler.s
SRC_C = $(CUBE_DEVICE_DIR)/Source/Templates/system_stm32f0xx.c \
        $(wildcard $(OS_DIR)/*.c)
SRC_LD = $(CUBE_DIR)/Projects/STM32F030R8-Nucleo/Templates/TrueSTUDIO/STM32F030R8-Nucleo/STM32F030R8_FLASH.ld

# Defines required by included libraries
DEF = -DSTM32F030x8

include ../cm-makefile/common.mk
include ../cm-makefile/openocd.mk

# Disable GCC warnings which are triggered by the STM32CubeF0 SDK
WARNFLAGS += -Wno-undef -Wno-conversion
