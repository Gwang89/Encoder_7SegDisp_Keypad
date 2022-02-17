/*
 * LS7366R.c
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
 *
 */
#include <msp430.h>
#include "usciSpi.h"
#include "LS7366R.h"

#ifndef LS7366R_C_
#define LS7366R_C_


/************************************************************************************
* Function: LS7366Rinit
* -Initialization function. Sets Filter Div and Counter size and Enables counter.
* argument:
*   none
* return: none
* Author: Andrew Friesen
* Date: Feb 1, 2022
************************************************************************************/
void LS7366Rinit(void)
{
    unsigned char data[1];
    //Configure GPIO for Slave Select
    P7DIR |= LS7366R_SS;
    LS7366R_SS_DISABLE;

    //Configure MDR0 + MDR1
    *data=QUAD_MODE_3 | FCLKDIV; //4x Quadrature, Free-running counter, no index, async index, filter clock div=2
    LS7366Rwrite(MDR0,data);

    *data=(4-COUNTER_SIZE); //4-byte counter mode, count enabled
    LS7366Rwrite(MDR1,data);

}
/************************************************************************************
 * Function: LS7366Rclear
 * -Clears register contents to 0.
 *
 * argument: unsigned char reg: the register to be cleared
 *           Valid Registers: MDR0, MDR1, CNTR, and STR
 *
 * returns 0 for success, 1 for Invalid register
 * Author: Andrew Friesen
 * Date: Feb 2, 2022
 ************************************************************************************/
unsigned char LS7366Rclear(unsigned char reg)
{
    if(reg==MDR0||reg==MDR1||reg==CNTR||reg==STR) //If writing to a valid register
    {
        LS7366R_SS_ENABLE;

        usciB1SpiPutChar(CLR_REG|reg);
        WAIT_FOR_RX;

        LS7366R_SS_DISABLE;
        return 0;
    }
    else
    {
        //Invalid register
        return 1;
    }

}
/************************************************************************************
 * Function: LS7366Rload
 * -Loads values from DTR into Counter or from Counter into OTR.
 * argument: unsigned char reg: the register to be loaded
 *           Valid Registers: CNTR and OTR
 *
 * returns 0 for success, 1 for Invalid register
 * Author: Andrew Friesen
 * Date: Feb 2, 2022
 ************************************************************************************/
unsigned char LS7366Rload(unsigned char reg)
{
    if(reg==CNTR||reg==OTR) //If writing to a valid register
    {
        LS7366R_SS_ENABLE;

        usciB1SpiPutChar(LOAD_REG|reg);
        WAIT_FOR_RX;

        LS7366R_SS_DISABLE;
        return 0;
    }
    else
    {
        //Invalid register
        return 1;
    }

}
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
unsigned char LS7366Rread(unsigned char reg, unsigned char *dataIn)
{

    if(reg==MDR0||reg==MDR1||reg==STR)//If you're reading an 1 byte register
    {
        //Send command followed by 1 dummy byte to receive the data byte
        LS7366R_SS_ENABLE;
        usciB1SpiPutChar(RD_REG|reg);
        WAIT_FOR_RX;

        usciB1SpiPutChar(DUMMY_BYTE);
        WAIT_FOR_RX;

        *dataIn=UCB1RXBUF;
        LS7366R_SS_DISABLE;

        return 0;
    }
    else if(reg==CNTR||reg==OTR||reg==DTR)//If you're reading an n-byte register
    {
        //Send a read command followed by n dummy bytes to receive the n data bytes
        LS7366R_SS_ENABLE;
        usciB1SpiPutChar(RD_REG|reg);
        WAIT_FOR_RX;

        int i;
        for(i=0;i<COUNTER_SIZE;i++)
        {
            usciB1SpiPutChar(DUMMY_BYTE);
            WAIT_FOR_RX;

            dataIn[COUNTER_SIZE - 1 - i]=UCB1RXBUF;
        }

        LS7366R_SS_DISABLE;
        return 0;
    }
    else
    {
        //Invalid register
        return 1;
    }
}
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
unsigned char LS7366Rwrite(unsigned char reg, unsigned char *dataOut)
{
    int i=0;
    if(reg==MDR0||reg==MDR1)//If you're writing to a 1 byte register
    {
        //send a write command followed by 1 data byte
        LS7366R_SS_ENABLE;
        usciB1SpiPutChar(WR_REG|reg);
        WAIT_FOR_RX;

        usciB1SpiPutChar(*dataOut);
        WAIT_FOR_RX;

        LS7366R_SS_DISABLE;
        return 0;
    }
    else if(reg==DTR)//If you're writing to an n-byte register
    {
        //Send a write command followed by n data bytes
        LS7366R_SS_ENABLE;
        usciB1SpiPutChar(WR_REG|reg);
        WAIT_FOR_RX;

        for(i=0;i<COUNTER_SIZE;i++)
        {
            usciB1SpiPutChar(dataOut[i]);
            WAIT_FOR_RX;
        }

        LS7366R_SS_DISABLE;
        return 0;
    }
    else
    {
        //Invalid register
        return 1;
    }
}


#endif /* LS7366R_C_ */
