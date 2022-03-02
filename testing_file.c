//
// Created by campell on 2022-03-01.
//
#include <stdio.h>

int testing_function(){

    unsigned char checksum = 0x00;
    unsigned char trimmed_mem_addr;
    int write_address = 0xFFA066BC; // 1111 1111 1010 0000 0110 0110 1011 1100
    // anticipated checksum: 0x85

    checksum = 0x00;
    // byte 1 is the address MSB, byte 4 is the LSB
    for (int i = 3 ; i >= 0 ; i--){

        trimmed_mem_addr = (write_address >> (8*i)) & 0xFF;
        checksum ^= trimmed_mem_addr;
        printf("transmitted byte: %#x\n", trimmed_mem_addr);

    }
    printf("checksum: %#x\n", checksum);

    return 0;
}
