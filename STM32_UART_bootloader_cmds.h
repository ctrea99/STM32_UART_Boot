
// pre-defined bytes used in STM32 communication
#define ACK  0x79
#define NACK 0x1F
#define UART_INIT_BYTE 0x7F
#define WRITE_MEM_CMD 0x31
#define ERASE_MEM_CMD 0x44
#define GO_CMD 0x21
#define GET_CMD 0x00

// number of bytes written to STM32 during one transaction (must be a multiple of 4)
// NOTE: Must be 128 to correspond to the page size in flash memory (see STM doc RM0367 pg.67)
#define NUM_BYTES_TX 128

// size of STM32 internal flash memory [# pages]
#define FLASH_MEM_SIZE 1024

// maximum number of flash memory pages that can be erased in one command
#define MAX_NUM_ERASE_PAGES 512

// function prototypes
int UART_init_bootloader(int device_handle);
int UART_write_memory(int device_handle, int write_address, unsigned char tx_data[]);
int UART_jump_to_address(int device_handle, int exe_addr);
int UART_erase_memory(int device_handle, int start_page, int num_pages);
int UART_get_info(int device_handle);