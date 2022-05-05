

#ifndef STM32_UART_BOOT_STM32_UART_BOOT_H
#define STM32_UART_BOOT_STM32_UART_BOOT_H

// pre-defined bytes used in STM32 communication
// TODO: Change _CMD_2 values to bit-wise complement of _CMD_1 values
#define ACK  0x79
#define NACK 0x1F
#define UART_INIT_BYTE 0x7F
#define WRITE_MEM_CMD 0x31
#define ERASE_MEM_CMD 0x44
#define GO_CMD 0x21
#define GET_CMD 0x00

// size of STM32 internal flash memory [# pages]
#define FLASH_MEM_SIZE 1024
#define MAX_NUM_ERASE_PAGES 512


// memory address STM32 software will be written to
// NOTE: Likely has to correspond to page boundary of flash memory (0x0800 0000 + n*0x80)
// (see STM doc RM0367 pg.67)
#define WRITE_ADDR_START 0x08000000


#define ERROR -1

// number of bytes written to STM32 during one transaction (must be a multiple of 4)
// NOTE: Must be 128 to correspond to the page size in flash memory (see STM doc RM0367 pg.67)
#define NUM_BYTES_TX 128


#define SOFTWARE_FILE_PATH "/home/campell/Documents/crea/absat/software/STM32_UART_Boot/IEB_basic_functionality.bin"

int STM32_UART_boot();
int UART_write_memory(int device_handle, int write_address, short int num_bytes_transmitted, unsigned char tx_data[]);
int UART_jump_to_address(int device_handle, int exe_addr);
int UART_erase_memory(int device_handle, int start_page, int num_pages);
int UART_get_info(int device_handle);

int STM32_wipe_flash_mem(int device_handle);

#endif //STM32_UART_BOOT_STM32_UART_BOOT_H
