

#define ERROR -1

// memory address STM32 software will be written to
// NOTE: Likely has to correspond to page boundary of flash memory (0x0800 0000 + n*0x80)
// (see STM doc RM0367 pg.67)
#define WRITE_ADDR_START 0x08000000

// file path of software binary to be uploaded to STM32
//#define SOFTWARE_FILE_PATH "/home/campell/Documents/crea/absat/software/STM32_UART_Boot/IEB_basic_functionality.bin"
#define SOFTWARE_FILE_PATH "/home/campell/Documents/crea/absat/software/STM32_UART_Boot/STM32L073RZ_NUCLEO.bin"

// function prototypes
int STM32_UART_boot();
int STM32_wipe_flash_mem(int device_handle);


