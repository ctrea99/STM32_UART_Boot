/*
 *  UART Driver
 *  Campbell Rea, AlbertaSAT
 *  Feb 22, 2022
 *
 *  Code is heavily influenced by:
 *  https://www.cmrr.umn.edu/~strupp/serial.html#1
 *
 *
 *  Known issues:
 *  - receives 0xD as 0xA, no other bytes are affected
 *
 */

#include "UART_driver.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

// Handles low-level details for transmitting/receiving data
// through UART interface
// For testing purposes, just use FTDI VCP drivers

int init_UART_interface(char* device_file){

    struct termios port_options;
    int device_handle;

    device_handle = open(device_file, O_RDWR | O_NOCTTY | O_NDELAY);
    if (device_handle == -1){
        printf("Error: unable to open device %s", device_file);
        return ERROR;
    }
    else {
        // restore blocking behaviour of port (Rx: wait until characters come in, until timeout, or until error)
        fcntl(device_handle, F_SETFL, 0);
    }

    // Set serial port options

    tcgetattr(device_handle, &port_options);    // get current options of the port

    // set the input/output baud rates specified by BAUD_RATE
    cfsetispeed(&port_options, B19200);
    cfsetospeed(&port_options, B19200);

    // enable the receiver and set local mode
    port_options.c_cflag |= (CLOCAL | CREAD);

    // ******** Set UART port data format ******** //
    // NOTE: STM32 UART1 interface: 8E1
    port_options.c_cflag &= ~CSIZE;  // mask the character size bits
    port_options.c_cflag |= CS8;     // set 8 bits per word
    port_options.c_cflag |= PARENB;  // enable parity bit
    //port_options.c_cflag &= ~PARENB; // disable parity bit
    port_options.c_cflag &= ~PARODD; // set even parity
    port_options.c_cflag &= ~CSTOPB; // set single stop bit

    port_options.c_cflag &= ~CRTSCTS;   // disable hardware flow control

    // set raw input mode (pass characters as they are received)
    port_options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // check and strip parity bit from input
    // Note: RX module is stripping MSB of transmitted word when parity stripping enabled
    //port_options.c_iflag |= (INPCK | ISTRIP);
    port_options.c_iflag &= ~ISTRIP; // keep parity bit
    port_options.c_iflag |= INPCK;   // check parity bit

    // disable software flow control
    port_options.c_iflag &= ~(IXON | IXOFF | IXANY);

    // enable raw output mode
    port_options.c_oflag &= ~OPOST;

    // set read timeouts (https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c)
    port_options.c_cc[VMIN] = 1;    //
    port_options.c_cc[VTIME] = 10;  // wait up to 1 second for each byte


    // set new options for the port
    tcsetattr(device_handle, TCSANOW, &port_options);

    printf("Device configuration complete\n");

    return device_handle;
}


int close_UART_interface(int device_handle){
    close(device_handle);
    return 0;
}


int UART_Tx(int device_handle, unsigned char tx_data){

    ssize_t bytes_transmitted = 0;


    // Note: RX module is stripping MSB of transmitted word
    // printf("Data transmitted: %#x\n", tx_data);

    // transmit tx_data to UART port
    bytes_transmitted = write(device_handle, &tx_data, 1);

    if (bytes_transmitted < 0){
        printf("Failed to transmit data\n");
        return ERROR;
    }

    return 0;
}


unsigned char UART_Rx(int device_handle){

    // note: UART Rx port does not timeout yet

    ssize_t bytes_received = 0;
    unsigned char rx_data = 0;


    bytes_received = read(device_handle, &rx_data, 1);

    //printf("Data received: %#x\n", rx_data);

    if (bytes_received < 0){
        printf("Failed to read data\n");
        return ERROR;
    }

    return rx_data;
}