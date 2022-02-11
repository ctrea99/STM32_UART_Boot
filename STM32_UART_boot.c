

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

    // handle for STM32 software file
    FILE *software_file_id;


    // set STM32 reset
    // set STM32 Boot0 pin HIGH
    // unset STM32 reset


    // Transmit UART_INIT_BYTE to initialize UART interface
    // Wait for ACK from STM32 (STM32 now able to receive commands)

    // Import STM32 software .bin file
    software_file_id = fopen(SOFTWARE_FILE_PATH, "rb");

    if (software_file_id == NULL){
        printf("The file %s could not be opened\n", SOFTWARE_FILE_PATH);
        return ERROR;
    }

    // Transmit contents of .bin file to STM32

    int num_bytes_read = 0;     // number of bytes read from STM32 software file
    int write_status   = 0;
    short int buffer [BYTES_TRANSMITTED];

    num_bytes_read = fread(buffer, 1, BYTES_TRANSMITTED, software_file_id);

    while (num_bytes_read > 0){

        // transmit bytes to STM32 flash memory
        write_status = UART_write_memory();

        if (write_status == ERROR){
            return ERROR;
        }

        // read in next few bytes from software .bin file
        num_bytes_read = fread(buffer, 1, BYTES_TRANSMITTED, software_file_id);
    }

    // Jump to address within STM32
    UART_jump_to_address();



    fclose(software_file_id);

    return 0;
}


int UART_write_memory(int write_address, short int num_bytes_transmitted){

    // transmit write memory command (0x31) + 0xCE

    // wait for ACK from STM32

    // transmit write address (4 bytes)
    // transmit checksum byte (XOR of addr bytes)

    // wait for ACK from STM32
    // check for NACK

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
