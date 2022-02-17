/*
 * usciSpi.c
 *
 * create a proper file header for the C module
 */

#include <msp430.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <usciSpi.h>

// create a function header that describes the function and how to use it. Provide an example function call.
/************************************************************************************
* Function: ucsiB1SpiInit
* - configures B1 SPI controlling the programmers model using the arguments passed by the caller.

* Arguments: (unsigned char) spiMst  - 0 is for slave configuration, otherwise it is a master
*            (unsigned int)  sclkDiv - clock signal divisor.
*            (unsigned char) sclkMode - 0 to 3, to set the capture/change timing and inactive high/low
*            (unsigned char) spiLookBack - 1 enables the transmitter output is internally fed back to the receiver.
* return: none
* Author: Gwang Sik Kim
* Date: Jan 22, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void usciB1SpiInit(unsigned char spiMST, unsigned int sclkDiv, unsigned char sclkMode, unsigned char spiLoopBack){

    P4SEL |= MOSI_B1 | CLK_B1 | MISO_B1;

    UCB1CTL1 |= UCSWRST;                      	// **Put state machine in USCI reset while you intitialize it**

    //master mode config
    if(spiMST){
        // spiMST is TRUE (1 or greater than that)
        UCB1CTL0 |= UCMST; // master mode
    }
    else{
        // spiMST is FALSE, equal to 0
        UCB1CTL0 &= ~UCMST; // slave mode
    }
    UCB1CTL1 &= ~UCSWRST;

    // divisor config
    usciB1SpiClkDiv(sclkDiv);// call usciB1SpiClkDiv

    UCB1CTL1 |= UCSWRST;
    // sclkMode config
    switch(sclkMode){
    case 0:
        UCB1CTL0 &= ~UCCKPH; // change -> capture
        UCB1CTL0 &= ~UCCKPL; // inactive low
        break;
    case 1:
        UCB1CTL0 &= ~UCCKPH; // change -> capture
        UCB1CTL0 |= UCCKPL;  // inactive high
        break;
    case 2:
        UCB1CTL0 |= UCCKPH;  // capture -> change
        UCB1CTL0 &= ~UCCKPL;  // inactive low
        break;
    case 3:
        UCB1CTL0 |= UCCKPH;  // capture -> change
        UCB1CTL0 |= UCCKPL;  // inactive high
        break;
    default: // default SMCLK
        UCB1CTL0 |= UCCKPH;  // capture -> change
        UCB1CTL0 |= UCCKPL;  // inactive high
    }


    // the rest of the settings... assumptions as default.
	// configure the control registers using the input arguments. See user manual, lecture notes and programmers model
	UCB1CTL0 |= UCMSB | UCSYNC; // falling edge change, inactive high, MSB first, synchronous mode
	UCB1CTL0 &= ~(UC7BIT);// 8 bit
	UCB1CTL0 &= ~UCMODE_3;
    UCB1CTL1 |= UCSSEL_3;  // SMCLK
	
	if (spiLoopBack){
	    UCB1STAT |= UCLISTEN; // the transmitter output is internally fed back to the receiver.
	}
	else{
	    UCB1STAT &= ~UCLISTEN;
	}

	
	// configure the SPI B1 pins with PxSEL register


	UCB1CTL1 &= ~UCSWRST;                     	// **Initialize USCI state machine**  take it out of reset
}


// provide function header
/************************************************************************************
* Function: usciB1SpiClkDiv
* - to configure the bit clock pre-scaler, UCB1BR1 and UCB1BR0 as 2 bytes.
*   BR1 is high byte and BR0 is low byte.
*   f_bitClk = f_BRCLK / UCBR

* Arguments: (unsigned int)  sclkDiv - divisor of the clock frequency
* return: none
* Author: Gwang Sik Kim
* Date: Jan 22, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
// this function is complete. Understand what it is doing.  Call it when SCLKDIV needs to be changed in Lab.
void usciB1SpiClkDiv(unsigned int sclkDiv){

    UCB1CTL1 |= UCSWRST;                        // you always need to put state machine into reset when configuring USC module

    UCB1BR0 = (sclkDiv&0xFF);                   // 2
    UCB1BR1 = (sclkDiv>>8);                     //

    UCB1CTL1 &= ~UCSWRST;                       // **Initialize USCI state machine**
}


// when the TXBUFFER is ready load it.    txByte-->TXBUFFER
// provide a function header
/************************************************************************************
* Function: usciB1SpiPutChar
* - polling TXIFG and if it's HIGH, the argument from the caller is passed to the TXBUF to transmit.

* Arguments: (unsigned char)  txByte
* return: none
* Author: Gwang Sik Kim
* Date: Jan 22, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void usciB1SpiPutChar(char txByte) {

    //UCB1IFG |= UCTXIFG;
    // same structure is re-used, adopted from UART driver...
    while (!(UCB1IFG & UCTXIFG)); // polling... is this efficient ? wait about 8 clock cycles...
    UCB1TXBUF = txByte;  // if TXBUFF ready then transmit a byte by writing to it
	
}

/************************************************************************************
* Function: ucsiB1SpiTxString
* - To transmit a string, this function transmits each character received from the caller
*   until it detects NULL character, as defined. (terminate)

* Arguments: (unsigned char)  txByte
* return: none
* Author: Gwang Sik Kim
* Date: Jan 22, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int usciB1SpiTxString(char* txChar){

    // complete this function
    unsigned int i = 0;

    // while not a null character
    while(txChar[i] != '\0'){
        // transmit a character with ucsiA1UartTxChar and increment the pointer
        usciB1SpiPutChar(txChar[i]);
        i++;
    }



    return i;
}


/************************************************************************************
* Function: usciB1SpiTxBuffer
* - buffer, which is a string from the caller, is sent to TxString function
* - and also control SS signal as Slave Select.

* Arguments: (unsigned char)  txByte
* return: none
* Author: Gwang Sik Kim
* Date: Jan 22, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int usciB1SpiTxBuffer(char* buffer, int bufLen) {
    int length = 0;

    length = usciB1SpiTxString(buffer);

    return (length);
}


/************************************************************************************
* Function: usciB1SpiTxBuffer
* - configures

* Arguments: (unsigned char)  txByte
* return: none
* Author: Gwang Sik Kim
* Date: Jan 22, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int numStringToInt(const char* rxString, char* spiTxBuffer){
    unsigned char i = 0;
    char tempChar=0;
    int tempInt = 0;
    while( *(rxString+i) != '\0'){
        //*(spiRxBuffer + i) = atoi(*(rxString + i));
        //spiRxBuffer[i] = atoi(&rxString[i]);

        //temp = atoi(&rxString[i]);
        //spiRxBuffer[i] = (char) temp;

        tempChar = rxString[i];

        tempInt= (char) atoi(&tempChar);
        spiTxBuffer[i] = (char)tempInt;
        //spiTxBuffer[i] = (char) atoi(&tempChar);

        //tempChar = *(rxString + i);
        //spiTxBuffer[i] = atoi(&tempChar);


        //spiTxBuffer[i]= atoi((rxString + i));



        i++;
    }


    spiTxBuffer[i] = 0; // the ending of the string.

    return i; // return 0 if string is NULL, or string's length.
}

#pragma vector=USCI_B1_VECTOR
__interrupt void usciB1SpiIsr(void) {
    //static unsigned char k = 0;
	
// UCB1IV interrupt handler. __even_in_range will optimize the C code so efficient jumps are implemented.
  switch(__even_in_range(UCB1IV,4)) // this will clear the current highest priority flag. TXIFG or RXIFG.
  {
  	  case 0: break;                          	// Vector 0 - no interrupt
  	  case 2:                                 	// Vector 2 - RXIFG. Highest priority
		// process RXIFG
  	      //usciB1SpiPutChar((unsigned char)UCB1RXBUF);
  	      //spiRxBuffer[RxBufIdx] = (char) UCB1RXBUF;
  	      //RxBufIdx++;


  	      // this works but not beautiful!
  	      //if( k == RxBufIdx)

  	      // it does not work
  	      //if(UCB1STAT && UCBUSY){
          //    P4OUT |= BIT0;
  	      //}


  	      // UART.h is needed.
  	      //if (rxBuffer[RxBufIdx] == '\0'){
  	      //    P6OUT |= BIT0;
  	      //}


  		  break;

  	  case 4:									// Vector 4 - TXIFG
  		//k++;
		//process TXIFG (careful)
  	    //P6OUT |= BIT0;

  		  break;

  	  default: break; 
  }
}
