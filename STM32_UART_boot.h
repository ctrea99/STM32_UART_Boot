

#ifndef STM32_UART_BOOT_STM32_UART_BOOT_H
#define STM32_UART_BOOT_STM32_UART_BOOT_H

#define ACK  0x79
#define NACK 0x1F
#define UART_INIT_BYTE 0x7F
#define ERROR -1

#define BYTES_TRANSMITTED 1

#define SOFTWARE_FILE_PATH "D:\\Campbells_Stuff\\University_of_Alberta\\AlbertaSAT\\Software\\STM32_UART_boot\\STM32_UART_Boot\\IEB_basic_functionality.bin"

int STM32_UART_boot();
int UART_write_memory(int write_address, short int num_bytes_transmitted);
int UART_jump_to_address();

#endif //STM32_UART_BOOT_STM32_UART_BOOT_H
