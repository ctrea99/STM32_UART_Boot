//
// Created by Campbell on 2022-02-10.
//

#ifndef STM32_UART_BOOT_UART_DRIVER_H
#define STM32_UART_BOOT_UART_DRIVER_H

int init_UART_interface(char* device_file);
int close_UART_interface(int device_handle);
int UART_Tx(int device_handle, unsigned char tx_data);
unsigned char UART_Rx(int device_handle);

#define ERROR -1
#define BAUD_RATE B19200


#endif //STM32_UART_BOOT_UART_DRIVER_H
