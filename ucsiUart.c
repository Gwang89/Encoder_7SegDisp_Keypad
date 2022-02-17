/*************************************************************************************************
 * ucsiUart.c
 * - C implementation or source file for MSP430 UCSI UART A1
 *
 *  Author: Greg Scutt
 *  Created on: March 1, 2017
 *  Modified: February 26th, 2018
 **************************************************************************************************/

#include <msp430.h>
#include <stdio.h>
#include "ucsiUart.h"
#include "usciSpi.h"



/************************************************************************************
* Function: ucsiA1UartInit
* - configures UCA1 UART to use SMCLK, no parity, 8 bit data, LSB first, one stop bit
*  BAUD rate = 19.2Kbps with 16xoversampling.
*  assumes SMCLK = 2^20 Hz.
* argument:
* Arguments: none, but baud rate would be useful
*
* return: none
* Author: Greg Scutt
* Date: March 1st, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void ucsiA1UartInit(){

	// READ THIS --> You must use UCSI Control BITS defined in msp430.h !!
	// In fact you must use pre-defined control bits whenever possible. no obscure HEX codes allowed anymore.

	P4SEL |= TXD_A1 | RXD_A1;
	UCA1CTL1 |= UCSWRST; 			// USCI A1  Software Reset Enabled
	                                // it allows to modify MCTL, BR registers.

	//********************

	UCA1CTL1 	|= 	UCSSEL_2; 		// select SMCLK. User is responsible for setting this rate.

	// UCSSEL_2 = 0x80, 10b -> SMCLK
	// UCSSEL_3 = 0xC0, 11b -> SMCLK as well.
	// UCSSEL__SMCLK can be used as well, defined in MSP430.h ...

	UCA1CTL0	 = 	0; 				// RESET UCA1CTL0 before new configuration
	UCA1CTL0	&=	~UCPEN			// No Parity
				&	~UCMSB			// LSB First
				&	~UC7BIT			// 8 bits of data
				&	~UCSPB			// 1 stop bit
				&	~UCSYNC;		// UART Mode, Asynchronous...

	// UCPAR == 0 -> Odd Parity
	// UCMODEx == 0 -> UART Mode ...

	// UCA1BRW = 0x03
	UCA1BR1 = 0x0;                  // calculate and confirm with table
	UCA1BR0 = 0x3;                  // calculate and confirm with table

	// here, hexa numbers are used for control bit ...
	// is there a literal constant list for UCA1BRW? something like UCBR_3???

	//UCA1MCTL = 0x63;	            // calculate and confirm with table.
	UCA1MCTL = UCBRF_6 | UCBRS_1 | UCOS16;
	// from the table...

	// UCBRx = 3 --> UCA1BR1 = 0x0, UCA1BR0 = 0x3;
	// UCBRSx = 1
	// UCBRFx = 6
    // UCOS16 = 1 (oversampling)
	// --> UCA1MCTL = (0x6) << 4 | (0x1) << 1 | 0x1;
	// --> 0x63...

	UCA1CTL1 	&= ~UCSWRST; 		//  configured. take state machine out of reset.
	}


/************************************************************************************
* Function: ucsiA1UartTxChar
* - writes a single character to UCA1TXBUF if it is ready
* argument:
* Arguments: txChar - byte to be transmitted
*
* return: none
* Author: Greg Scutt
* Date: March 1st, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void ucsiA1UartTxChar(unsigned char txChar) {

	while (!(UCA1IFG & UCTXIFG)); // is this efficient ?
	UCA1TXBUF = txChar;	 // if TXBUFF ready then transmit a byte by writing to it
}


/************************************************************************************
* Function: ucsiA1UartTxString
* - writes a C string of characters, one char at a time to UCA1TXBUF by calling
*   ucsiA1UartTxChar. Stops when it encounters  the NULL character in the string
*   does NOT transmit the NULL character
* argument:
* Arguments: txChar - pointer to char (string) to be transmitted
*
* return: number of characters transmitted
* Author: Greg Scutt
* Date: March 1st, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int ucsiA1UartTxString(unsigned char *txChar){

	// complete this function
    unsigned int i = 0;

	// while not a null character
    while(txChar[i] != '\0'){
        // transmit a character with ucsiA1UartTxChar and increment the pointer
        ucsiA1UartTxChar(txChar[i]);
        i++;
    }



    return i;
}




/************************************************************************************
* Function: usciA1UartTXBuffer
* - transmit bufLen characters from buffer, and return the number of bytes transmitted
* Arguments: char* buffer - the string to be transmitted
*            int bufLen   - the length of the string
*
* return: the number of bytes transmitted
* Author: Gwang Sik Kim
* Date: Jan 13, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int usciA1UartTxBuffer(unsigned char *buffer, int bufLen){

    int length = 0;
    length = ucsiA1UartTxString(buffer);

    char dispNumLen[40];
    sprintf(dispNumLen, "%d characters are transmitted", length);
    ucsiA1UartTxString((unsigned char*)dispNumLen);

    return (length * 2 + 1);
    // each alphabet has 2 Bytes, and START, STOP BITS, PARITY BIT are up to 3~4 bits. max 1 byte.
}


/************************************************************************************
* Function: usciA1UartGets
* - receive a string entered from the console, and store it into an array pointed
*   to by rxString. returns pointer to rxString or NULL if unsuccessful.
* Arguments: char* - rxString
*
* return: char* - pointer to rxString
* Author: Gwang Sik Kim
* Date: Jan 14, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
char* usciA1UartGets(char* rxString){
    /*char* rxStringPtr;

    if (*rxString == '\0'){
        rxStringPtr = '\0';
    }
    else{
        rxStringPtr = rxString;
    }
     */
    unsigned int i=0;
    unsigned int j=0;
    do{
        while (!(UCA1IFG & UCRXIFG)); //wait
        // wait until the user passes return
        // why is it skipping sometimes ???
        // turn off INTERRUPT!!!!
        rxBuffer[i++] = UCA1RXBUF;
        //ucsiA1UartTxChar(UCA1RXBUF);

    }while((UCA1RXBUF != ASCII_CR) && ((UCA1RXBUF != ASCII_LF))&& (i < BUFF_SIZE));

    if (i >= BUFF_SIZE){
        // exceed the size
        *rxString = ASCII_NULL;
    }
    else{
        rxBuffer[i - 1] = ASCII_NULL; // null insertion

        //copy rxBuffer to rxString
        for (j = 0; j<i ; j++){
            *(rxString + j) = rxBuffer[j];
        }
    }
    // pass the contents of rxBuffer to rxString

    return rxString;
}





#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
  switch(__even_in_range(UCA1IV,4))
  {
  case 0:
      break;
  // no interrupt.
  case 2:
      // RXIFG
	// do something.
      //ucsiA1UartTxString((unsigned char*)UCA1RXBUF);
      //while ((UCA1IFG & UCRXIFG));

      // UART RXBUF data -> SPI tossing...
      usciB1SpiPutChar((unsigned char)UCA1RXBUF);
      //ucsiA1UartTxChar((unsigned char)UCA1RXBUF);
      //usciA1UartGets(rxBuffer);


      break;
  case 4:
      break;
  // TXIFG
  //ucsiA1UartTxChar(testChar);
  default:
      break;
  }
}

