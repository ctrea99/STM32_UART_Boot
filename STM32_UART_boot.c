

#include "STM32_UART_boot.h"
#include "STM32_UART_bootloader_cmds.h"
#include "UART_driver.h"
#include <stdio.h>


/*
 *  Current Issues:
 *    - How to wait for ACK/NACK from STM32
 *
 *
 */

// TODO: Move wait for ACK/NACK to separate function?


int STM32_UART_boot(){

    char device_file[256] = "/dev/ttyUSB0";    // device file for serial port
    int device_handle;         // serial port handle
    FILE *software_file_id;    // handle for STM32 software file

    // initialize serial port
    device_handle = init_UART_interface(device_file);


    // set STM32 reset
    // set STM32 Boot0 pin HIGH
    // unset STM32 reset


    // initialize UART interface on STM32
    UART_init_bootloader(device_handle);

    // get info on STM32 supported commands
    UART_get_info(device_handle);

    // Erase existing contents of STM32 flash memory
    STM32_wipe_flash_mem(device_handle);


    // Import STM32 software .bin file
    software_file_id = fopen(SOFTWARE_FILE_PATH, "rb");
    if (software_file_id == NULL){
        printf("Error: The file %s could not be opened\n", SOFTWARE_FILE_PATH);
        return ERROR;
    }

    // Transmit contents of .bin file to STM32

    size_t num_bytes_read  = 0;     // number of bytes read from STM32 software file
    int  write_status      = 0;
    int  write_address     = WRITE_ADDR_START;  // STM32 memory address bytes will be written to
    unsigned char tx_buffer [NUM_BYTES_TX];              // buffer to store data that will be written to memory


    int counter = 0;     // used to track software progression

    // read first line from STM32 software file
    num_bytes_read = fread(tx_buffer, 1, NUM_BYTES_TX, software_file_id);

    // loop until entire STM32 software has been written
    while (num_bytes_read > 0){

        // known byte sequence for test purposes
        //buffer[0] = 0xAA;
        //buffer[1] = 0xFF;
        //buffer[2] = 0xCC;
        //buffer[3] = 0x56;

        counter++;
        printf("Counter: %4d,\tWrite address: %#x\n", counter, write_address);

        // transmit bytes to STM32 flash memory
        write_status = UART_write_memory(device_handle, write_address, tx_buffer);
        if (write_status == ERROR){
            printf("Error: Unable to write byte to memory\n");
            return ERROR;
        }


        // Optional: Verification of flash memory contents after write operation
        if (MEM_READBACK_EN){
            int readback_status = STM32_verify_memory(device_handle, write_address, tx_buffer);

            if (readback_status == ERROR){
                printf("Error: Memory verification failed\n");
                return ERROR;
            }
        }

        // increment write address
        write_address += NUM_BYTES_TX;

        // read in next few bytes from software .bin file
        // TODO: Not sure if this automatically pads with 0's if num_bytes_read < NUM_BYTES_TX
        num_bytes_read = fread(tx_buffer, 1, NUM_BYTES_TX, software_file_id);
    }

    printf("Info: Write completed\n");


    // Jump to address within STM32 (begin execution)
    printf("Info: Jumping to software\n");
    UART_jump_to_address(device_handle, WRITE_ADDR_START);

    printf("Info: STM32 boot completed\n");


    fclose(software_file_id);      // close STM32 software file
    close_UART_interface(device_handle);  // close serial port

    return 0;
}



// completely erase all pages of STM32 internal flash memory
int STM32_wipe_flash_mem(int device_handle){

    int erase_status;
    int current_page_num = 0;

    printf("Info: Beginning mass erase of STM32 flash memory\n");

    for (int i = 0; i < FLASH_MEM_SIZE / MAX_NUM_ERASE_PAGES; i++){
        erase_status = UART_erase_memory(device_handle, current_page_num, MAX_NUM_ERASE_PAGES);
        if (erase_status != 0){
            printf("Error: Could not erase STM32 flash memory\n");
            return ERROR;
        }
        current_page_num += MAX_NUM_ERASE_PAGES;
    }

    if ((FLASH_MEM_SIZE % MAX_NUM_ERASE_PAGES) != 0) {
        erase_status = UART_erase_memory(device_handle, current_page_num, FLASH_MEM_SIZE % MAX_NUM_ERASE_PAGES);
        if (erase_status != 0) {
            printf("Error: Could not erase STM32 flash memory\n");
            return ERROR;
        }
    }

    printf("Info: Memory erase completed\n");

    return 0;
}

// verify contents of STM32 flash memory with expected values
int STM32_verify_memory(int device_handle, int read_address, unsigned char tx_buffer[]){

    int  read_status;
    unsigned char rx_buffer[NUM_BYTES_TX];

    read_status = UART_read_memory(device_handle, read_address, rx_buffer);
    if (read_status != 0){
        printf("Error: Could not read flash memory contents\n");
        return ERROR;
    }

    // compare flash memory contents with intended transmitted data
    for (int i = 0; i < NUM_BYTES_TX; i++){
        if (tx_buffer[i] != rx_buffer[i]){
            printf("Error: Flash memory contents do not match expected value\n");
            printf("\tMemory address:\t%#x\n", read_address + i);
            printf("\tExpected value:\t%#x\n", tx_buffer[i]);
            printf("\tRead value:\t\t%#x\n", rx_buffer[i]);
            return ERROR;
        }
    }

    return 0;
}
