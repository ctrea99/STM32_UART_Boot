set(FIND_FTD2XX_PATHS ${STM32_UART_Boot_SOURCE_DIR}/libraries/FTDI_D2XX_driver)

find_path(FTD2XX_INCLUDE_DIR ftd2xx.h
        PATH_SUFFIXES include
        PATHS ${FIND_FTD2XX_PATHS})

find_library(FTD2XX_LIBRARY
        NAMES ftd2xx
        PATH_SUFFIXES lib
        PATHS ${FIND_FTD2XX_PATHS})
