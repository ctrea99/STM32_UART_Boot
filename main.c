#include <stdio.h>
#include "STM32_UART_boot.h"
#include "UART_driver.h"
#include <termios.h>
#include <unistd.h>

int main() {

    STM32_UART_boot();


    /*
    device_handle = init_UART_interface(device_file);

    UART_Tx(device_handle, 0xD);
    rx_data = UART_Rx(device_handle);

    printf("rx_data: %#x\n", rx_data);
    */

    /*

    // Note: error sending 0xD
    int error_counter = 0;

    for (unsigned char tx_data = 0; tx_data < 0xFF; tx_data++){
        UART_Tx(device_handle, tx_data);
        rx_data = UART_Rx(device_handle);

        printf("tx_data: %#x\n", tx_data);
        printf("rx_data: %#x\n", rx_data);
        printf("---------------\n");

        if (tx_data != rx_data){
            printf("Error: tx_data (%#x) does not match rx_data (%#x)\n", tx_data, rx_data);
            error_counter++;
        }

    }

    printf("Total errors: %d\n", error_counter);
     */



    return 0;
}
