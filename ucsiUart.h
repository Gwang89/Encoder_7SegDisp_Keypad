/*************************************************************************************************
 * ucsiUart.h
 * - - C interface file for MSP430 UCSI UART A1, A0
 *
 *  Author: Greg Scutt
 *  Created on: March 1, 2017
 *  Modified: Feb 19, 2018
 **************************************************************************************************/

#ifndef UCSIUART_H_
#define UCSIUART_H_

#define     TXD_A1          BIT4            //Transmit Data on P4.4
#define     RXD_A1          BIT5            //Recieve Data on P4.5
#define     BUFF_SIZE       100
#define     ASCII_CR        0x0D
#define     ASCII_LF        0x0A
#define     ASCII_NULL      0x00



// structure
// should be above prototype, so that linker understands what structure is being used. ..

// prototype
void ucsiA1UartInit();
void ucsiA1UartTxChar(unsigned char txChar);
int ucsiA1UartTxString(unsigned char* txChar);
int usciA1UartTxBuffer(unsigned char* buffer, int bufLen);
char* usciA1UartGets(char* rxString);


//global variable
char rxBuffer[BUFF_SIZE];
//CMD cmds[MAX_CMDS];
#endif /* UCSIUART_H_ */
