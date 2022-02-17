/*
 * usciSpi_Lab.h
 *
 *  Created on: 2022. 1. 22.
 *      Author: GwangSik
 */

#ifndef USCISPI_H_
#define USCISPI_H_

// define
#define     CLK_B1                  BIT3             // CLK PORT on P4.3
#define     MOSI_B1                 BIT1            // Master Out on P4.1
#define     MISO_B1                 BIT2            // Master In on P4.2
#define     MASTER_MODE             0x1
#define     CLOCK_DIV_1             0x1
#define     CLOCK_DIV_2             0x2
#define     CLOCK_DIV_4             0x4
#define     CLOCK_DIV_16            16
#define     INACTIVE_LOW            0x0
#define     DATA_CAPTURE_RISING     0x2
#define     LOOPBACK_ENABLE         0x1
#define     LOOPBACK_DISABLE        0x0
#define     EX523                   0
#define     EX524                   0
#define     BUFF_SIZE       100
// structure

// prototype
void usciB1SpiInit(unsigned char spiMST, unsigned int sclkDiv, unsigned char sclkMode, unsigned char spiLoopBack);
void usciB1SpiClkDiv(unsigned int sclkDiv);
void usciB1SpiPutChar(char txByte);

int usciB1SpiTxString(char* txChar);
int usciB1SpiTxBuffer(char* buffer, int bufLen);
int numStringToInt(const char* rxString, char* spiTxBuffer);

// global variable
char spiRxBuffer[BUFF_SIZE];
// char* is 'pointer' to pointer, so it's double pointer,
// which is 16 bytes size. because it's address...
// declare it (char), not (char*).

unsigned char RxBufIdx; // index
unsigned char k;


#endif /* USCISPI_H_ */
