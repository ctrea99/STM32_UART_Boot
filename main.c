#include <stdio.h>
#include "STM32_UART_boot.h"
#include "UART_driver.h"
#include "testing_file.h"
#include <termios.h>
#include <unistd.h>

int main() {

    STM32_UART_boot();
    //testing_function();

    return 0;
}
