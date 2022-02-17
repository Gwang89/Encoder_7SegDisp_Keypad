/*************************************************************************************************
 * nok5110LCD.h
 * - C interface file for NOKIA 5110 LCD.
 *
 *  Author: Greg Scutt
 *  Created on: Feb 20th, 2017
 **************************************************************************************************/

// nok5110LCD pin connectivity --> to MSP-EXP430F5529LP EVM.
// Assumes UCB1SI  is used.
//  8-LIGHT  	-->  	no connection necassary unless you want it on
//  7-SCLK  	-->  	MS430EVM  P4.3 or UCB1CLK
//  6-DN(MOSI)  -->  	MS430EVM  P4.1 or UCB1SIMO
//  5-D/C'  	-->  	MS430EVM  P4.2. 	Kept as I/O pin !!
//  4-RST'  	-->  	MS430EVM or supply VSS.  Can tie to I/O pin if user wants. data sheet says RESET is necassary .. but?
//                  --> P2.3 is selected.
//  3-SCE'  	-->  	MS430EVM  P4.0. chip select 	Kept as I/O pin !!
//  2-GND  		-->  	MS430EVM or supply VSS
//  1-VCC  		-->  	MS430EVM or supply 3V3
//                  --> P2.4 is selected.


/*
 * ========= PINOUT Configuration =========
 *  1       2       3       4       5       6       7       8
 * Vcc      Gnd     SCE     RST     D/C     MOSI    SCLK    LED
 * P2.4     GND     P4.0    P2.3    P4.2    P4.1    P4.3    N/A (Open)
 *
 * ========= =================== =========
 *
 *
 */


#ifndef nok5110LCD_H_
#define nok5110LCD_H_

// nok5110 pin --> msp430 PORT4 bit position
#define SCLK  	            BIT3
#define DAT_CMD             BIT2
#define SIMO 	            BIT1
#define	SCE   	            BIT0
// PORT 2 PIN OUT
#define LCD_VCC             BIT4
#define LCD_RST             BIT3

// constants for cmdType argument passed to the nokLcdWrite function
#define DC_CMD  0	// command control
#define DC_DAT  1	// data control

// NOKIA 5110 LCD row,col MAX
#define LCD_MAX_COL 84  	// 	max # of columns.  84 pixels in x direction
#define LCD_MAX_ROW 48     	// 	max # of row.  48 pixels in y direction
#define LCD_MAX_BANK 6

// NOKIA 5110 LCD controller instructions for initialization
#define LCD_NORMAL_DISP      	0x0C // normal display control
#define LCD_ALL_SEG_DISP        0x09 // normal display control
#define LCD_EXT_INSTR 			0x21 // enable extended instruction set
#define LCD_HORI_ADDRESSING     0x21 // enable extended instruction set
#define LCD_VERTI_ADDRESSING    0x23 // enable extended instruction set
#define LCD_BASIC_INSTR      	0x20 // enable basic instruction set
#define LCD_SET_SYSBIAS       	0x13 // set system bias mode
#define LCD_SET_OPVOLT     		0xBC // set operation voltage for contrast ctrl
#define LCD_SET_TEMPCTRL       	0x04 // set coeff 2
#define LCD_SET_YRAM          	0x40 // set Y address of RAM
#define LCD_SET_XRAM          	0x80 // set X address of RAM

#define LCD_ROW_IN_BANK 8 	    // 8 rows in a bank. 6 banks, so  8x6 = 48 rows of pixels. y coordinate

// NOKIA LCD PINOUT Assertion / De-assertion
#define _NOK_LCD_RST_LOW                P2OUT &= ~LCD_RST
#define _NOK_LCD_RST_HIGH               P2OUT |= LCD_RST
#define _NOK_LCD_VCC_LOW                P2OUT &= ~LCD_VCC
#define _NOK_LCD_VCC_HIGH               P2OUT |= LCD_VCC
#define _NOK_LCD_SCE_LOW                P4OUT &= ~SCE
#define _NOK_LCD_SCE_HIGH               P4OUT |= SCE
#define _NOK_LCD_DAT_CMD_LOW            P2OUT &= ~DAT_CMD
#define _NOK_LCD_DAT_CMD_HIGH           P2OUT |= DAT_CMD

// nokLcdDrawScrnLine argument
#define HORIZONTAL_LINE     0x0
#define VERTICAL_LINE       0x2

int count;


// prototype
void nokLcdInit(void);
void spiTxByte(char txData);
unsigned char nokLcdSetPixel(unsigned char xPix, unsigned char yPix);
void nokLcdWrite(char lcdByte, char cmdType);
void nokLcdClear(void);
void nokLcdFull(void);
char nokLcdDrawScrnLine(unsigned char position, unsigned lineDir);
int nokLcdDrawLine(int x1, int y1, int x2, int y2);
char nokLcdDrawScrnLine2(unsigned char position, unsigned lineDir);
void nokLcdLoading(void);

#endif /* nok5110LCD_H_ */
