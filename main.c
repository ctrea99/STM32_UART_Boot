#include <stdio.h>
//#include "./libraries/FTDI_D2XX_driver/ftd2xx.h"
#include "STM32_UART_boot.h"



#include <stdarg.h>
#include <stdlib.h>
#include <windows.h>
#include <windef.h>
#include <winnt.h>
#include <winbase.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#define FTD2XX_STATIC
#include "ftd2xx.h"



int main() {

    int test;


    FT_HANDLE ftHandle;
    FT_STATUS ftStatus;

    ftStatus = FT_Open(0, &ftHandle);
    if (ftStatus == FT_OK){
        printf("It opened\n");
    }
    else {
        printf("It did not open\n");
    }

    //test = STM32_UART_boot();

    return 0;
}
