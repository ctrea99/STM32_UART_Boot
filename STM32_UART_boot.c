

#include "STM32_UART_boot.h"
#include "UART_driver.h"
#include <stdio.h>


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


    // Import STM32 software .bin file
    software_file_id = fopen(SOFTWARE_FILE_PATH, "rb");
    if (software_file_id == NULL){
        printf("The file %s could not be opened\n", SOFTWARE_FILE_PATH);
        return ERROR;
    }

    // Transmit contents of .bin file to STM32

    size_t num_bytes_read = 0;     // number of bytes read from STM32 software file
    int write_status   = 0;
    short int buffer [NUM_BYTES_TX];

    // read first line from STM32 software file
    num_bytes_read = fread(buffer, 1, NUM_BYTES_TX, software_file_id);

    // loop until entire STM32 software has been written
    while (num_bytes_read > 0){

        // transmit bytes to STM32 flash memory
        // write_status = UART_write_memory();
        // increment write address

        if (write_status == ERROR){
            return ERROR;
        }

        // read in next few bytes from software .bin file
        num_bytes_read = fread(buffer, 1, NUM_BYTES_TX, software_file_id);
    }

    // Jump to address within STM32 (begin execution)
    UART_jump_to_address();



    fclose(software_file_id);      // close STM32 software file
    close_UART_interface(device_handle);  // close serial port

    return 0;
}


int UART_write_memory(int device_handle, int write_address, short int num_bytes_transmitted){

    unsigned char rx_data;     // received data from serial port

    // transmit write memory command (0x31) + 0xCE
    UART_Tx(device_handle, WRITE_MEM_CMD_1);
    UART_Tx(device_handle, WRITE_MEM_CMD_2);

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
    // transmit checksum byte (XOR of addr bytes)

    for (int i = 0 ; i < 4 ; i++){

    }

    // wait for ACK from STM32


    // send number of bytes to be transmitted N

    // transmit N+1 bytes of data
    // transmit checksum (XOR of all data bytes)

    // wait for ACK
    // check for NACK


    return 0;
}


int UART_jump_to_address(){

    // Transmit Go command (0x21) + 0xDE

    // wait for ACK

    // transmit jump address (4 bytes)
    // transmit checksum

    // wait for ACK
    // check for NACK

    return 0;
}
