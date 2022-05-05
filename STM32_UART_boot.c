

#include "STM32_UART_boot.h"
#include "UART_driver.h"
#include <stdio.h>
#include <stdlib.h>


/*
 *  Current Issues:
 *    - How to wait for ACK/NACK from STM32
 *
 *
 */

int STM32_UART_boot(){

    char device_file[256] = "/dev/ttyUSB0";    // device file for serial port
    int device_handle;         // serial port handle
    unsigned char rx_data;     // received data from serial port
    int result = 0;            // status bit for function calls

    FILE *software_file_id;    // handle for STM32 software file

    // initialize serial port
    device_handle = init_UART_interface(device_file);


    // set STM32 reset
    // set STM32 Boot0 pin HIGH
    // unset STM32 reset


    // initialize UART interface on STM32
    result = UART_Tx(device_handle, UART_INIT_BYTE);
    if (result < 0){
        printf("Error: can't transmit data\n");
        return ERROR;
    }

    // Wait for ACK from STM32 (STM32 now able to receive commands)
    rx_data = UART_Rx(device_handle);
    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return ERROR;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return ERROR;
    }
    else {
        printf("Info: STM32 UART port initialized\n");
    }

    // get info on STM32 supported commands
    UART_get_info(device_handle);


    // Erase existing contents of STM32 flash memory
    STM32_wipe_flash_mem(device_handle);

    return 0;



    // Import STM32 software .bin file
    software_file_id = fopen(SOFTWARE_FILE_PATH, "rb");
    if (software_file_id == NULL){
        printf("Error: The file %s could not be opened\n", SOFTWARE_FILE_PATH);
        return ERROR;
    }

    // Transmit contents of .bin file to STM32

    size_t num_bytes_read = 0;     // number of bytes read from STM32 software file
    int write_status      = 0;
    int write_address     = WRITE_ADDR_START;  // STM32 memory address bytes will be written to
    unsigned char buffer [NUM_BYTES_TX];

    int counter = 0;

    // read first line from STM32 software file
    num_bytes_read = fread(buffer, 1, NUM_BYTES_TX, software_file_id);

    // loop until entire STM32 software has been written
    while (num_bytes_read > 0){

        buffer[0] = 0xAA;
        buffer[1] = 0xFF;
        buffer[2] = 0xCC;
        buffer[3] = 0x56;

        counter++;
        printf("Counter: %d,\tWrite address: %#x\n", counter, write_address);

        // transmit bytes to STM32 flash memory
        write_status = UART_write_memory(device_handle, write_address, NUM_BYTES_TX, buffer);

        // increment write address
        write_address += NUM_BYTES_TX;

        if (write_status == ERROR){
            printf("Error: Unable to write byte to memory\n");
            return ERROR;
        }

        // read in next few bytes from software .bin file
        num_bytes_read = fread(buffer, 1, NUM_BYTES_TX, software_file_id);
    }

    printf("Write completed\n");


    // Jump to address within STM32 (begin execution)
    UART_jump_to_address(device_handle, WRITE_ADDR_START);

    printf("Finished?\n");



    fclose(software_file_id);      // close STM32 software file
    close_UART_interface(device_handle);  // close serial port

    return 0;
}


int UART_write_memory(int device_handle, int write_address, short int num_bytes_transmitted, unsigned char tx_data[]){

    unsigned char rx_data;           // received data from serial port
    unsigned char trimmed_mem_addr;  // trimmed write address to tx individual byte
    unsigned char checksum;          // STM32 checksum byte

    // transmit write memory command (0x31) + 0xCE
    UART_Tx(device_handle, WRITE_MEM_CMD);
    UART_Tx(device_handle, ~WRITE_MEM_CMD);

    // wait for ACK from STM32
    rx_data = UART_Rx(device_handle);
    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return ERROR;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return ERROR;
    }


    // transmit write address (4 bytes)
    // byte 1 is the address MSB, byte 4 is the LSB

    checksum = 0x00;

    for (int i = 3 ; i >= 0 ; i--){
        // extract ith byte from write_address
        trimmed_mem_addr = (write_address >> (8*i)) & 0xFF;
        checksum ^= trimmed_mem_addr;

        UART_Tx(device_handle, trimmed_mem_addr);
    }
    // transmit checksum byte (XOR of address bytes)
    UART_Tx(device_handle, checksum);

    // wait for ACK from STM32
    rx_data = UART_Rx(device_handle);
    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return ERROR;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return ERROR;
    }


    checksum = 0x00;  // reset checksum

    // send number of bytes to be transmitted N
    UART_Tx(device_handle, NUM_BYTES_TX - 1);
    checksum ^= (NUM_BYTES_TX - 1);


    // transmit N+1 bytes of data
    for(int i = 0; i < NUM_BYTES_TX; i++){
        UART_Tx(device_handle, tx_data[i]);
        checksum ^= tx_data[i];
    }

    // transmit checksum (XOR of all data bytes)
    UART_Tx(device_handle, checksum);

    // wait for ACK, check for NACK
    rx_data = UART_Rx(device_handle);
    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return ERROR;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return ERROR;
    }

    return 0;
}


int UART_jump_to_address(int device_handle, int exe_addr){

    unsigned char rx_data;
    unsigned char checksum;
    unsigned char trimmed_mem_addr;

    // Transmit Go command (0x21) + 0xDE
    UART_Tx(device_handle, GO_CMD);
    UART_Tx(device_handle, ~GO_CMD);

    // wait for ACK, check for NACK
    rx_data = UART_Rx(device_handle);
    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return ERROR;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return ERROR;
    }

    // transmit jump address (4 bytes)
    checksum = 0x00;

    for (int i = 3 ; i >= 0 ; i--){
        // extract ith byte from write_address
        trimmed_mem_addr = (exe_addr >> (8*i)) & 0xFF;
        checksum ^= trimmed_mem_addr;

        UART_Tx(device_handle, trimmed_mem_addr);
    }
    // transmit checksum byte (XOR of address bytes)
    UART_Tx(device_handle, checksum);

    // wait for ACK, check for NACK
    rx_data = UART_Rx(device_handle);
    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return ERROR;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return ERROR;
    }

    return 0;
}



int UART_erase_memory(int device_handle, int start_page, int num_pages){

    unsigned char rx_data;
    unsigned char checksum;
    unsigned char trimmed_rx_data;

    if (num_pages > MAX_NUM_ERASE_PAGES){
        printf("Error: Attempting to erase too many pages in one command\n");
        return ERROR;
    }

    // Transmit Erase Memory command (0x44) + 0xBB
    UART_Tx(device_handle, ERASE_MEM_CMD);
    UART_Tx(device_handle, ~ERASE_MEM_CMD);

    // wait for ACK, check for NACK
    rx_data = UART_Rx(device_handle);
    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return ERROR;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return ERROR;
    }

    checksum = 0x00;

    // transmit number of pages to be erased
    trimmed_rx_data = ((num_pages - 1) >> 8) & 0xFF;
    UART_Tx(device_handle, trimmed_rx_data);
    checksum ^= trimmed_rx_data;

    trimmed_rx_data = (num_pages - 1) & 0xFF;
    UART_Tx(device_handle, trimmed_rx_data);
    checksum ^= trimmed_rx_data;


    // transmit the page numbers to be erased
    // 2 bytes, MSB first
    for (int i = start_page; i < start_page + num_pages; i++){

        trimmed_rx_data = (i >> 8) & 0xFF;
        UART_Tx(device_handle, trimmed_rx_data);
        checksum ^= trimmed_rx_data;

        trimmed_rx_data = i & 0xFF;
        UART_Tx(device_handle, trimmed_rx_data);
        checksum ^= trimmed_rx_data;
    }

    // transmit checksum
    UART_Tx(device_handle, checksum);

    // wait for ACK, check for NACK
    rx_data = UART_Rx(device_handle);
    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return ERROR;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return ERROR;
    }


    return 0;
}


int UART_get_info(int device_handle){

    unsigned char rx_data;
    int num_bytes_rx;
    unsigned char *STM_info;

    // Transmit Get command (0x00) + 0xFF
    UART_Tx(device_handle, GET_CMD);
    UART_Tx(device_handle, ~GET_CMD);

    // wait for ACK, check for NACK
    rx_data = UART_Rx(device_handle);
    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return ERROR;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return ERROR;
    }

    // get number of bytes to be received
    num_bytes_rx = (int)UART_Rx(device_handle) + 1;


    STM_info = (unsigned char *)malloc(num_bytes_rx * sizeof(char));

    // Get bootloader version and supported commands
    for (int i = 0; i < num_bytes_rx; i++){
        STM_info[i] = UART_Rx(device_handle);
    }

    // wait for ACK, check for NACK
    rx_data = UART_Rx(device_handle);
    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return ERROR;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return ERROR;
    }

    printf("Bootloader Version: %#x\n", STM_info[0]);

    printf("Supported Commands:\n");
    for (int i = 1; i < num_bytes_rx; i++){
        printf("%3d: %#x\n", i, STM_info[i]);
    }


    free(STM_info);

    return 0;
}

// completely erase all pages of STM32 internal flash memory
int STM32_wipe_flash_mem(int device_handle){

    int erase_status;
    int current_page_num = 0;

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

    return 0;
}
