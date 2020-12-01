#include "st7735_interface.h"


int st7735_read_config(int _fd) {
    unsigned char iBuf[4000];
    unsigned char oBuf[4000];

}

/*void manage_ms5611(int _fd) {
    unsigned char iBuf[4000];
    unsigned char oBuf[4000];

    //print config
    oBuf[0] = 0x05;
    SPI_RW(_fd, 1, oBuf, 1, iBuf);
    oBuf[0] = 0x35;
    SPI_RW(_fd, 1, oBuf, 1, iBuf + 1);
    oBuf[0] = 0x15;
    SPI_RW(_fd, 1, oBuf, 1, iBuf + 2);
    printf("SR1: %02x\n", iBuf[0]);
    printf("SR2: %02x\n", iBuf[1]);
    printf("SR3: %02x\n", iBuf[2]);

    if ( !(iBuf[0] & 0x2) ) {
        printf("Enabling writing..");
        oBuf[0] = 0x06;
        SPI_RW(_fd, 1, oBuf, 0, iBuf);

        // checking
        oBuf[0] = 0x05;
        SPI_RW(_fd, 1, oBuf, 1, iBuf);
        if ( !(iBuf[0] & 0x2) ) {
            printf("failed :: (%u)\n", iBuf[0]);
        } else {
            printf("success\n");
        }
    }

    if ( !(iBuf[0] & 0x2) ) {
        printf("Enabling writing..");
        oBuf[0] = 0x06;
        SPI_RW(_fd, 1, oBuf, 0, iBuf);

        // checking
        oBuf[0] = 0x05;
        SPI_RW(_fd, 1, oBuf, 1, iBuf);
        if ( !(iBuf[0] & 0x2) ) {
            printf("failed :: (%u)\n", iBuf[0]);
        } else {
            printf("success\n");
        }
    }

    //erase sector
    oBuf[0] = 0x20;
    oBuf[1] = 0x10;
    oBuf[2] = 0x0;
    oBuf[3] = 0x0;
    SPI_RW(_fd, 4, oBuf, 0, iBuf);

    usleep(100000); // 10 ms sleep




    for ( int i = 0 ; i < 50; ++i ) {

        oBuf[0] = 0x05;
        SPI_RW(_fd, 1, oBuf, 1, iBuf);

        if ( !(iBuf[0] & 0x2) ) {
            oBuf[0] = 0x06;
            SPI_RW(_fd, 1, oBuf, 0, iBuf);

            // checking
            oBuf[0] = 0x05;
            SPI_RW(_fd, 1, oBuf, 1, iBuf);
        }


        oBuf[0] = 0x02;
        oBuf[1] = 0x10;
        oBuf[2] = 0x0;
        oBuf[3] = i;
        oBuf[4] = i + 0x20;
        SPI_RW(_fd, 5, oBuf, 0, iBuf);

        printf("%02x ", oBuf[4]);
    }

    printf("\n");

    usleep(1000000); // 10 ms sleep


    oBuf[0] = 0x03;
    oBuf[1] = 0x10;
    oBuf[2] = 0x0;
    oBuf[3] = 0x0;

    SPI_RW(_fd, 4, oBuf, 50, iBuf);
    uint8_t fFailed = 0;
    for ( int i = 0 ; i < 50; ++ i ) {
        printf("%02x ", iBuf[i]);
        if ( iBuf[i] != i + 0x20 ) {
            fFailed = 1;
        }
    }
    printf("\n");
    if ( fFailed ) {
        printf("EEPROM Test has failed!\n");
        return;
    }
    printf("[SUC] Test has succeeded.\n");
}*/