/*
 * LS7366R.h
 *
 *  Created on: Jan. 31, 2022
 *      Author: andrew_friesen
 *
 *  Breakout board connections to MSP430
 *
 *  S1 Slave Select ->  P7.0 GPIO
 *  MOSI            ->  P4.1 UCB1MOSI
 *  MISO            ->  P4.2 UCB1MISO
 *  SCLK            ->  P4.3 UCB1CLK
 *  5V              ->  3.3V
 *  GND             ->  GND
 */

#ifndef LS7366R_H_
#define LS7366R_H_

#define LS7366R_SS_ENABLE P7OUT &= ~BIT0
#define LS7366R_SS_DISABLE P7OUT |= BIT0
#define LS7366R_SS BIT0

#define WAIT_FOR_RX while (!(UCB1IFG&UCRXIFG)); UCB1IFG &= ~UCRXIFG

#define COUNTER_SIZE 4
#define DUMMY_BYTE 0x00

//********Instruction Reg (IR) Defines******************

// IR=(7:6)Op-codes | (5:3)Registers | (2:0) Don't care

//Registers
#define MDR0     (0x08) //Mode Reg 0
#define MDR1     (0x10) //Mode Reg 1
#define DTR      (0x18) //Data In Buffer Reg
#define CNTR     (0x20) //Counter
#define OTR      (0x28) //Data Out Buffer Reg
#define STR      (0x30) //Status Reg

//Op-codes
// B7 B6
#define CLR_REG  (0x00) //Clear register   0000
#define RD_REG   (0x40) //Read register    0100
#define WR_REG   (0x80) //Write register   1000
#define LOAD_REG (0xC0) //Load register to/from buffer    1100

//********Mode Reg 0 (MDR0) Defines******************

// MDR0=(7) Filter clock div | (6) Index sync | (5:4) Index mode | (3:2) Count mode | (1:0) Quadrture mode

#define FCLKDIV      (0x80) //Filter clock div
#define INDEX_SYNC   (0x40)
#define INDEX_MODE   (0x30)
#define COUNT_MODE   (0x0C)
#define QUAD_MODE    (0x03)

#define INDEX_MODE_0 (0x00) //Disable index
#define INDEX_MODE_1 (0x10) //Index loads counter from DTR
#define INDEX_MODE_2 (0x20) //Index resets counter
#define INDEX_MODE_3 (0x30) //Index loads OTR with counter

#define COUNT_MODE_0 (0x00) //Free-running
#define COUNT_MODE_1 (0x04) //Single-cycle
#define COUNT_MODE_2 (0x08) //Range-limit
#define COUNT_MODE_3 (0x0C) //Modulo-n

#define QUAD_MODE_0  (0x00) //No quadrature (A=clk, B=direction)
#define QUAD_MODE_1  (0x01) //1x 1 count per 4 quadrature events
#define QUAD_MODE_2  (0x02) //2x 2 counts per 4 quadrature events
#define QUAD_MODE_3  (0x03) //4x 4 counts per 4 quadrature events

//********Mode Reg 1 (MDR1) Defines******************

// MDR0=(7:3) Don't care | (2) Enable Count | (1:0) Counter size bytes

#define COUNT_ENABLE  (0x04)
#define COUNT_SIZE    (0x03) //Counter size in bytes

#define COUNT_SIZE_0  (0x00) //4 bytes (32-bit)
#define COUNT_SIZE_1  (0x01) //3 bytes (24-bit)
#define COUNT_SIZE_2  (0x02) //2 bytes (16-bit)
#define COUNT_SIZE_3  (0x03) //1 bytes (8-bit)

//********Status Reg (STR) Defines******************

#define CY  (0x80) //Carry
#define BW  (0x40) //Borrow
#define CMP (0x20) //Compare latch
#define IDX (0x10) //Index latch
#define CEN (0x08) //Count enable status
#define PLS (0x04) //Power loss indicator latch
#define UD  (0x02) //Count direction indicator
#define S   (0x01) //Sign bit


/************************************************************************************
* Function: LS7366Rinit
* -Initialization function. Sets Filter Div and Counter size and Enables counter.
* argument:
*   none
* return: none
* Author: Andrew Friesen
* Date: Feb 1, 2022
************************************************************************************/
void LS7366Rinit(void);

 /************************************************************************************
 * Function: LS7366Rclear
 * -Clears register contents to 0.
 *
 * argument: unsigned char reg: the register to be cleared
 *           Valid Registers: MDR0, MDR1, CNTR, and STR
 *
 * return: returns 0 for success, 1 for Invalid register
 *
 * Author: Andrew Friesen
 * Date: Feb 2, 2022
 ************************************************************************************/
 unsigned char LS7366Rclear(unsigned char reg);

/************************************************************************************
 * Function: LS7366Rload
 * -Loads values from DTR into Counter or from Counter into OTR.
 *
 * argument: unsigned char reg: the register to be loaded
 *           Valid Registers: CNTR and OTR
 *
 * return: returns 0 for success, 1 for Invalid register
 *
 * Author: Andrew Friesen
 * Date: Feb 2, 2022
 ************************************************************************************/
 unsigned char LS7366Rload(unsigned char reg);

/************************************************************************************
 * Function: LS7366Rread
 * -Reads contents of a register.
 * argument:-unsigned char reg: the register to be read
 *           Valid Registers: MDR0, MDR1, CNTR, OTR and STR
 *
 *          -unsigned char * dataIn: a pointer to a data buffer
 *
 * return: returns 0 for success, 1 for Invalid register
 *
 * Author: Andrew Friesen
 * Date: Feb 1, 2022
 ************************************************************************************/
unsigned char LS7366Rread(unsigned char reg, unsigned char *dataIn);

/************************************************************************************
 * Function: LS7366Rwrite
 * -Writes the contents of a register.
 * argument:-unsigned char reg: the register to be read
 *           Valid Registers:  MDR0, MDR1, and DTR
 *
 *          -unsigned char * dataOut: a pointer to a data buffer
 *
 * return: returns 0 for success, 1 for Invalid register
 *
 * Author: Andrew Friesen
 * Date: Feb 1, 2022
 ************************************************************************************/
unsigned char LS7366Rwrite(unsigned char reg, unsigned char *dataOut);




#endif /* LS7366R_H_ */
