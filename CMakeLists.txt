cmake_minimum_required(VERSION 3.21)
project(STM32_UART_Boot C)

set(CMAKE_C_STANDARD 99)

add_executable(main main.c STM32_UART_boot.c STM32_UART_boot.h UART_driver.c UART_driver.h)
