/*************************************************************************************************
 * nok5110LCD.c
 * - C implementation or source file for NOKIA 5110 LCD.
 *
 *  Author: Greg Scutt
 *  Created on: Feb 20th, 2017
 **************************************************************************************************/



// nok5110LCD pin connectivity --> to MSP-EXP430F5529LP EVM.
//  8-LIGHT  	-->  	no connection necassary
//  7-SCLK  	-->  	MS430EVM  P4.3 or UCB1CLK
//  6-DN(MOSI)  -->  	MS430EVM  P4.1 or UCB1SIMO
//  5-D/C'  	-->  	MS430EVM  P4.2. 	Kept as I/O pin !!
//  4-RST'  	-->  	MS430EVM or supply VSS
//  3-SCE'  	-->  	MS430EVM  P4.0.  	Kept as I/O pin !!
//  2-GND  		-->  	MS430EVM or supply VSS
//  1-VCC  		-->  	MS430EVM or supply 3V3



#include <msp430.h>
#include "nok5110LCD.h" // remove _lab from file names
#include <usciSpi.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// 2-D 84x6 array that stores the current pixelated state of the display.
// remember a byte (8 bits) sets 8 vertical pixels in a column allowing 8x6=48 rows
// note that this array is GLOBAL to this file only. In that way it is protected from access from other functions in files.
// said another way - it is a private global array with local scope to the file in which the defining declaration exists.
// we don't want other functions messing with the shadow RAM. This is the reason for static and for its dec/defn in the .c file
static unsigned char currentPixelDisplay[LCD_MAX_COL][LCD_MAX_ROW / LCD_ROW_IN_BANK];





/************************************************************************************
* Function: nokLcdInit
* - initialize NOKIA LCD following the timing diagram defined in Manual,
*   VCC goes HIGH and within t_VHRL RST should be low and return to high within t_WL_RST
* argument: (void) n/a
* return: (void) n/a
* Author: Greg Scutt
* Date: Feb 20th, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void nokLcdInit(void) {




   // Set DC and CE Low // because it's default as LOW, redundant..?
    _NOK_LCD_VCC_LOW;
    _NOK_LCD_RST_LOW;
    P2DIR   |= (LCD_RST | LCD_VCC); // OUTPUT

    /****************** ENSURE RESET TIMING IS CORRECT ******************/
    //P2OUT   &= ~(LCD_RST | LCD_VCC);
    //_NOK_LCD_RST_HIGH; // active low
    //_NOK_LCD_VCC_LOW;
    // equivalent?

    //_NOK_LCD_RST_LOW; // before VCC, RST can be low (set-up time 0s)
    _NOK_LCD_VCC_HIGH;
    _NOK_LCD_RST_HIGH; // return to HIGH.
    /*********************************************/


    // MAKE SURE this line has to be AFTER power/reset initialization.
    // weird behaviour may happen because of the signals going into the LCD before the proper power process.
    // gScutt.  do an SPI init with ucsiB1SpiInit  from ucsiSpi.h before this function call !!
    //P4OUT   |= (SCE | DAT_CMD);
    //P4DIR   |= (SCE | DAT_CMD);
    P2OUT   |= DAT_CMD;
    P2DIR   |= DAT_CMD;
    P4OUT   |= (SCE);
    P4DIR   |= (SCE);


    // send initialization sequence to LCD module
    nokLcdWrite(LCD_EXT_INSTR, DC_CMD);
    nokLcdWrite(LCD_SET_OPVOLT, DC_CMD);
    nokLcdWrite(LCD_SET_TEMPCTRL, DC_CMD);
    nokLcdWrite(LCD_SET_SYSBIAS, DC_CMD);
    nokLcdWrite(LCD_BASIC_INSTR, DC_CMD);
    nokLcdWrite(LCD_NORMAL_DISP, DC_CMD);

    //nokLcdClear(); // clear the display
}


/************************************************************************************
* Function: nokLcdWrite
* - performs write sequence to send data or command to nokLCD. Calls spiTxByte to transmit serially to nokLCD
* argument:
* Arguments: lcdByte - the 8 bit char (data or command) that is written to nokLCD.
* 			 cmdType - 0 - lcdByte is a cmd,   1 - lcdByte is data.
* return: none
* Author: Greg Scutt
* Date: Feb 20th, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void nokLcdWrite(char lcdByte, char cmdType) {
	
	// ********** complete this function. *************

	// check cmdType and output correct DAT_CMD signal to PORT4 based on it.
    if (cmdType == 1){
        // data
        _NOK_LCD_DAT_CMD_HIGH;
    }
    else if (cmdType == 0){
        // command
        _NOK_LCD_DAT_CMD_LOW;
    }

	// activate the SCE. the chip select
    _NOK_LCD_SCE_LOW;

	// transmit lcdByte with spiTxByte
    spiTxByte(lcdByte);

	// wait for transmission complete ?  If so, disable the SCE
    while (!(UCB1IFG & UCRXIFG));
    _NOK_LCD_SCE_HIGH;
    UCB1IFG &= ~UCRXIFG;
    // even if you turn off LOOPBACK and INTERRUPT,
    // hardware still receives the interrupt flag and set.
    // you need to check it and clear it...

}




/************************************************************************************
* Function: nokLcdSetPixel
* -
* argument:
*	xPos - The horizontal pixel location in the domain (0 to 83)
*	yPos - The vertical pixel location in the domain (0 to 47)
*
* return: 0 - pixel was valid and written.  1 - pixel not valid
* Author: Greg Scutt
* Date: Feb 20th, 2017
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
unsigned char nokLcdSetPixel(unsigned char xPos, unsigned char yPos) {
	unsigned char bank; // a bank is a group of 8 rows, selected by 8 bits in a byte

	// verify pixel position is valid
	if ((xPos < LCD_MAX_COL) && (yPos < LCD_MAX_ROW)) {

		// if-else statement avoids costly division
		if (yPos<8) bank = 0;
		else if (yPos<16) bank = 1;
		else if (yPos<24) bank = 2;
		else if (yPos<32) bank = 3;
		else if (yPos<40) bank = 4;
		else if (yPos<48) bank = 5;

		// set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
		nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
		nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);

		// update the pixel being set in currentPixelDisplay array
		currentPixelDisplay[xPos][bank] |= BIT0 << (yPos % LCD_ROW_IN_BANK); // i.e if yPos = 7 then BIT0 is left shifted 7 positions to be 0x80. nice
		nokLcdWrite(currentPixelDisplay[xPos][bank], DC_DAT); // write the data. this is DATA DC_DAT
		return 0;
	}
	return 1;
}




/************************************************************************************
* Function: nokLcdClear
* - clears all pixels on LCD diplay. results in blank display.
* argument:
*   none
* return: none
* Author: Gwang Sik Kim
* Date: Jan 30, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void nokLcdClear(void){
    unsigned char xPos=0;
    unsigned char bank; // a bank is a group of 8 rows, selected by 8 bits in a byte

    for (bank=0; bank<LCD_MAX_BANK; bank++){
        for (xPos=0; xPos<LCD_MAX_COL; xPos++){

            // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
            nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
            nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);

            currentPixelDisplay[xPos][bank] = 0; // CLEAR all data?
            nokLcdWrite(currentPixelDisplay[xPos][bank], DC_DAT); // write the data. this is DATA DC_DAT
        }
    }
}


/************************************************************************************
* Function: nokLcdFull
* - clears all pixels on LCD diplay. results in blank display.
* argument:
*   none
* return: none
* Author: Gwang Sik Kim
* Date: Jan 30, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void nokLcdFull(void){
    unsigned char xPos=0;
    unsigned char bank; // a bank is a group of 8 rows, selected by 8 bits in a byte

    for (bank=0; bank<LCD_MAX_BANK; bank++){
        for (xPos=0; xPos<LCD_MAX_COL; xPos++){

            // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
            nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
            nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);

            currentPixelDisplay[xPos][bank] = 0xFF; // CLEAR all data?
            nokLcdWrite(currentPixelDisplay[xPos][bank], DC_DAT); // write the data. this is DATA DC_DAT
        }
    }
}

/************************************************************************************
* Function: spiTxByte
* - if TXBUFFER is ready!!  then a byte is written to buffer and serialized on SPI UCB1. nothing else happens.
* argument:
*	txData - character to be serialized over SPI
*
* return: none
* Author: Gwang Sik Kim
* Date: Jan 30, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void spiTxByte(char txData)
{
	// transmit a byte on SPI if it is ready !!
    while (!(UCB1IFG & UCTXIFG)); // polling... is this efficient ? wait about 8 clock cycles...
    UCB1TXBUF = txData;  // if TXBUFF ready then transmit a byte by writing to it
}


/************************************************************************************
* Function: nokLcdDrawScrnLine
* - draw a lince horizontal / vertical
* argument:
*  (unsigned char) position - x or y position that the line is situated
*  (unsigned char) lineDir - horizontal or vertical
*
* return: (char) illegal state - if the lineDir is valid, return 0. otherwise, -1.
* Author: Gwang Sik Kim
* Date: Jan 30, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
char nokLcdDrawScrnLine(unsigned char position, unsigned lineDir){
    signed char illegal = -1;
    unsigned char orthoVector = 0;
    if (lineDir == HORIZONTAL_LINE){
        if(position < LCD_MAX_ROW){// in horizontal line, position refers to the y-position
            illegal = 0;
            // ack

            // V = 0. horizontal addressing
            while(orthoVector < LCD_MAX_COL){
                // y position fixed, x position ranges 0 to the max.
                nokLcdSetPixel(orthoVector,position);
                orthoVector++;
            }
        }
    }
    else if (lineDir == VERTICAL_LINE){
        if(position < LCD_MAX_COL){// in vertical line, position refers to the x-position
            illegal = 0;
            // ack

            while(orthoVector < LCD_MAX_ROW - 1){
                nokLcdSetPixel(position, orthoVector);
                orthoVector++;
            }
        }
    }
    return illegal;
}


/************************************************************************************
* Function: nokLcdDrawLine
* - draw a line in 2D space following the Brensenham Algorithm.
* argument:
*   (int) x1 - starting point x coordinate
*   (int) y1 - starting point y coordinate
*   (int) x2 - end point x coordinate
*   (int) y2 - end point y coordinate
*
* return: (int) illegal - if the input arguments are out of valid range, -1. if okay, 0.
* Author: Gwang Sik Kim
* Date: Jan 30, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int nokLcdDrawLine(int x1, int y1, int x2, int y2){// assume x1 < x2
    int illegal = -1;

    // verify pixel position is valid
    if ((x1 < LCD_MAX_COL) && (y1 < LCD_MAX_ROW)) {
        if ((x2 < LCD_MAX_COL) && (y2 < LCD_MAX_ROW)) {
            illegal = 0; // range valid
        }
    }

    if (illegal == 0){

        //referred to bresenham algorithm
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int fx = x2 < x1 ? -1 : 1; // if yes, -1. else, 1.
        int fy = y2 < y1 ? -1 : 1; // if yes, -1. else, 1.
        int newXPos = x1;
        int newYPos = y1;

        if (dx > dy){
            // width > height
            double discriminant  = (2 * dy) - dx; // discriminant1
            for (newXPos = x1; newXPos != x2 ; newXPos += fx){
                if (discriminant <0){
                    discriminant += 2 * dy;
                }
                else{
                    newYPos += fy;
                    discriminant += (2*dy - 2*dx);
                }

                if(nokLcdSetPixel(newXPos, newYPos)){
                    count++;
                }
            }
        }
        else{
            // width <= height
            double discriminant  = (2 * dx) - dy; // discriminant2
            for (newYPos = y1; newYPos != y2; newYPos += fy){
                if (discriminant <0){
                    discriminant += 2 * dx;
                }
                else{
                    newXPos += fx;
                    discriminant += (2*dx - 2*dy);
                }
                nokLcdSetPixel(newXPos, newYPos);
            }
        }

    }

    return illegal;
}





/*
char nokLcdDrawScrnLine2(unsigned char position, unsigned lineDir){
    signed char illegal = -1;
    unsigned char orthoVector = 0;
    unsigned char bank;
    unsigned char xPos = 0;
    if (lineDir == HORIZONTAL_LINE){
        if(position < LCD_MAX_ROW){// in horizontal line, position refers to the y-position
            illegal = 0;
            // ack

            // V = 0. horizontal addressing
            //while(orthoVector < LCD_MAX_COL){
                // y position fixed, x position ranges 0 to the max.
           //     nokLcdSetPixel(orthoVector,position);
           //     orthoVector++;
            //}
            // if-else statement avoids costly division
            if (position<8) bank = 0;
            else if (position<16) bank = 1;
            else if (position<24) bank = 2;
            else if (position<32) bank = 3;
            else if (position<40) bank = 4;
            else if (position<48) bank = 5;


            // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
            //nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
            //nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);


            // example?
            nokLcdWrite(0x20, DC_CMD); // 0x0C
            nokLcdWrite(0x90, DC_CMD); // 0x0C
            nokLcdWrite(0x0C, DC_CMD); // 0x0C
            //nokLcdWrite(0x20, DC_CMD); // 0x0C

            //nokLcdWrite(0x20, DC_CMD); // 0x0C
            for (xPos = 0; xPos < LCD_MAX_COL; xPos++){
                // update the pixel being set in currentPixelDisplay array
                //currentPixelDisplay[xPos][bank] |= BIT0 << (position % LCD_ROW_IN_BANK); // i.e if yPos = 7 then BIT0 is left shifted 7 positions to be 0x80. nice
                //nokLcdWrite(currentPixelDisplay[xPos][bank], DC_DAT); // write the data. this is DATA DC_DAT
                nokLcdWrite(0xAA, DC_DAT);
                //__delay_cycles(10000);
            }



        }
    }
    else if (lineDir == VERTICAL_LINE){
        if(position < LCD_MAX_COL){// in vertical line, position refers to the x-position
            illegal = 0;
            // ack


            while(orthoVector < LCD_MAX_ROW){
                // x position fixed, y position ranges 0 to the max.
                nokLcdSetPixel(position, orthoVector);
                orthoVector++;
            }
        }
    }


    return illegal;
}
*/

/************************************************************************************
* Function: nokLcdLoading
* - for the initial screen, display 'BCIT' alphabets.
* argument:
*       (void) n/a
* return: (void) n/a
* Author: Gwang Sik Kim
* Date: Feb 14, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void nokLcdLoading(void){
    unsigned char xPos = 28;
    unsigned char bank = 1;

    unsigned char loadingDat[40] = {0xFF, 0x81, 0x42, 0x3C, 0x00, 0xF8, 0x04, 0x02, 0x01, 0x00, 0x00, 0x01, 0xFF, 0x01, 0x00, 0x00, 0x01, 0xFF, 0x01, 0xFF, 0x81, 0x42, 0x3C, 0x00, 0x1F, 0x20, 0x40, 0x80, 0x00, 0x00, 0x80, 0xFF, 0x80, 0x00, 0x00, 0x00, 0xFF, 0x00};
    int i=0;
    for (i = 0; i< 19; i++){
        nokLcdWrite(LCD_SET_XRAM | xPos + i, DC_CMD);
        nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);
        currentPixelDisplay[xPos + i][bank] = loadingDat[i];
        nokLcdWrite(currentPixelDisplay[xPos + i][bank], DC_DAT);
    }

    bank = 2;
    for (i = 0; i< 19; i++){
        nokLcdWrite(LCD_SET_XRAM | xPos + i, DC_CMD);
        nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);
        currentPixelDisplay[xPos + i][bank] = loadingDat[i + 19];
        nokLcdWrite(currentPixelDisplay[xPos + i][bank], DC_DAT);
    }
}
