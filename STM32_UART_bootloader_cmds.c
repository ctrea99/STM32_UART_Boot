
#include "STM32_UART_bootloader_cmds.h"
#include "UART_driver.h"
#include <stdio.h>
#include <stdlib.h>


int UART_init_bootloader(int device_handle){

    unsigned char rx_data;     // received data from serial port
    int result = 0;            // status bit for function calls

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

    return 0;
}


int UART_write_memory(int device_handle, int write_address, unsigned char tx_data[]){

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

// Get bootloader version and list of supported commands
int UART_get_info(int device_handle){

    unsigned char rx_data;
    int           num_bytes_rx;
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
