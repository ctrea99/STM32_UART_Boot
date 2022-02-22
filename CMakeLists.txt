cmake_minimum_required(VERSION 3.21)
project(STM32_UART_Boot C)

set(CMAKE_C_STANDARD 99)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}\\cmake")


add_executable(main main.c STM32_UART_boot.c STM32_UART_boot.h UART_driver.c UART_driver.h)

include(FindPkgConfig)
find_package(FTD2XX REQUIRED)
include_directories(${FTD2XX_INCLUDE_DIR})
target_link_libraries(main ${FTD2XX_LIBRARY})



