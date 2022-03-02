

#ifndef STM32_UART_BOOT_STM32_UART_BOOT_H
#define STM32_UART_BOOT_STM32_UART_BOOT_H

#define ACK  0x79
#define NACK 0x1F
#define UART_INIT_BYTE 0x7F
#define WRITE_MEM_CMD_1 0x31
#define WRITE_MEM_CMD_2 0xCE
#define GO_CMD_1 0x21
#define GO_CMD_2 0xDE
#define ERROR -1

#define NUM_BYTES_TX 1

#define SOFTWARE_FILE_PATH "/home/campell/Documents/crea/absat/software/STM32_UART_Boot/IEB_basic_functionality.bin"

int STM32_UART_boot();
int UART_write_memory(int device_handle, int write_address, short int num_bytes_transmitted, unsigned char tx_data);
int UART_jump_to_address(int device_handle, int exe_addr);

#endif //STM32_UART_BOOT_STM32_UART_BOOT_H
