

#ifndef STM32_UART_BOOT_STM32_UART_BOOT_H
#define STM32_UART_BOOT_STM32_UART_BOOT_H

// pre-defined bytes used in STM32 communication
#define ACK  0x79
#define NACK 0x1F
#define UART_INIT_BYTE 0x7F
#define WRITE_MEM_CMD_1 0x31
#define WRITE_MEM_CMD_2 0xCE
#define GO_CMD_1 0x21
#define GO_CMD_2 0xDE

// memory address STM32 software will be written to
#define WRITE_ADDR_START 0x08000000

#define ERROR -1

// number of bytes written to STM32 during one transaction
// must be a multiple of 4
#define NUM_BYTES_TX 4

#define SOFTWARE_FILE_PATH "/home/campell/Documents/crea/absat/software/STM32_UART_Boot/IEB_basic_functionality.bin"

int STM32_UART_boot();
int UART_write_memory(int device_handle, int write_address, short int num_bytes_transmitted, unsigned char tx_data[]);
int UART_jump_to_address(int device_handle, int exe_addr);

#endif //STM32_UART_BOOT_STM32_UART_BOOT_H
