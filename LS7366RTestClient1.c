#include <msp430.h> 
#include "nok5110LCD.h"
#include "usciSpi.h"
#include "ucsiUart.h"
#include <string.h>
#include <stdio.h>
#include "LS7366R.h"
#include "usciCmdInterpreter.h"
#include "fediCmd.h"


#define     DELAY_1SEC  1000000
#define     CYCLE_COUNT 50000
//#define     CYCLE_COUNT 60000


// demo macro
#define     TEST1           0
#define     TEST2           1
#define     TEST3           0

// prototype
void initTA0(int cycle);

//global
//unsigned int xTest = 0, yTest = 0;
long signed int* posCount;
unsigned char bufferCNTR[4] = {0, 0, 0, 0};

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	

	// ======================== SPI config =================================
    // SPI initialize...
    const unsigned char spiMst = MASTER_MODE; // master
    //const unsigned int spiClkDiv = CLOCK_DIV_1;
    //const unsigned int spiClkDiv = CLOCK_DIV_2;
    //const unsigned int spiClkDiv = CLOCK_DIV_4;
    const unsigned int spiClkDiv = CLOCK_DIV_16;
    unsigned char const spiLoopBack = LOOPBACK_DISABLE;
    //unsigned char const spiLoopBack = LOOPBACK_ENABLE;
    const unsigned char sclkMode = DATA_CAPTURE_RISING | INACTIVE_LOW;  // UCCKPH = 1, capture > change, UCCKPL = 0, inactive low.
    usciB1SpiInit(spiMst, spiClkDiv, sclkMode, spiLoopBack);

    // SPI configured as...
    // 19200 Baud Rate, DIV = 16, MASTER, CAPTURE > CHANGE, Inactive Low...
    // Loopback Disabled.
    // P4.3 CLK, P4.1 MOSI
    // ======================== =========== =================================

    // ======================== UART config =================================
    // UART initialize...
    ucsiA1UartInit();
    char rxString[50];

    // ======================== =========== =================================

    CMD cmdList[MAX_CMDS];
    initCmdList(cmdList);

    //UCB1IE |= UCRXIE;
    __enable_interrupt();
	nokLcdInit();
	nokLcdClear();

    // start
    nokLcdLoading(); // BCIT

	__delay_cycles(DELAY_1SEC);
	nokLcdClear();

    // ======================== Timer config =================================

	unsigned int cycle = CYCLE_COUNT; // 1us -> 50000 count -> 50ms.
	initTA0(cycle);

    // ======================== ============ =================================

    // ======================== LS7366R init  =================================

	LS7366Rinit();

    // ======================== ============ =================================

	unsigned char bufferMDR0;
    unsigned char bufferMDR1;

    unsigned char bufferWrite[4] = {5, 6, 8, 9};
    int i;

    char* errorFlag;
    char dispMsg[40];
    int index = -2;
    int errIdx = -2;

    while(1){

        if(TEST1){
            // verify the regDataBuf is the same value with initialized MDR0, MDR1...
            LS7366Rread(MDR0, &bufferMDR0);        // MOSI 48 -> MISO 0x83
            LS7366Rread(MDR1, &bufferMDR1);        // MOSI 50 -> MISO 0x0

            //LS7366Rclear(CNTR);
            //LS7366Rread(CNTR, bufferCNTR);

            LS7366Rwrite(DTR, bufferWrite); // 98 05 06 08 09
            LS7366Rload(CNTR); // E0

            LS7366Rread(CNTR, bufferCNTR); // 60 - 05 06 08 09
            posCount = 0;

            // assumption: MSByte is transmitted first, needs to flip that order and store it.
            /*
            for (i = 0 ; i<4 ; i++){
                posCount |= bufferCNTR[3-i] * (BIT0 << (i * 8));
            }
            */
            //posCount = bufferCNTR[3] * 0x1 + bufferCNTR[2] * 0x100 + bufferCNTR[1]*0x10000 + bufferCNTR[0] * 0x1000000;
            posCount = *bufferCNTR;
        }

        else if(TEST2){

            LS7366Rclear(CNTR);
            //posCount = 0;
            while(1){
                //sprintf(dispMsg, "%ld\n", posCount);
                sprintf(dispMsg, "%ld\n", *posCount);
                ucsiA1UartTxString((unsigned char*)dispMsg);


            }

        }



        else if(TEST3){
            LS7366Rclear(CNTR);
            while(1){
                // initial display
                sprintf(dispMsg, "Waiting for the user...\n");
                ucsiA1UartTxString((unsigned char*)dispMsg);
                rewind(stdin);
                errorFlag = usciA1UartGets(rxString);
                if (*errorFlag == ASCII_NULL){
                    sprintf(dispMsg, "Error detected\n");
                    ucsiA1UartTxString((unsigned char*)dispMsg);
                }
                else{
                    sprintf(dispMsg, "User Enters a command... \n");
                    ucsiA1UartTxString((unsigned char*)dispMsg);
                    index = parseCmd(cmdList, rxString);
                    sprintf(dispMsg, "The command index is %d.\n", index);
                    ucsiA1UartTxString((unsigned char*)dispMsg);
                    errIdx = executeCmd(cmdList, index);
                    if(errIdx == -1){
                        sprintf(dispMsg, "The command arguments were not acceptable.\n");
                        ucsiA1UartTxString((unsigned char*)dispMsg);
                    }
                    else{
                        sprintf(dispMsg, "Successful.\n");
                        ucsiA1UartTxString((unsigned char*)dispMsg);
                    }
                }
            }
        }
    }

	return 0;
}






/*************************************************************************************
 * ISR Name         : timer0A1ISR
 * Description      : interrupted by the flag set by comparing CCR0
 *                    the timer TA0 Main Timer and channel 1 to 5 use this ISR.
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/
#pragma vector = TIMER0_A1_VECTOR
__interrupt void timer0A1ISR(void){
    static int count = 0;

    // this ISR starts when the timer counts up to cycle, 50000. which is 50ms.

    switch(count){
    case 0:// count = 50ms
        break;

    case 1:// count = 100ms
    // every 100ms, posCount updated.
        //LS7366Rread(CNTR, bufferCNTR);
        //*posCount = bufferCNTR
        LS7366Rread(CNTR, bufferCNTR); // read CNTR and update it.
        //posCount = bufferCNTR[0] * 0x1 + bufferCNTR[1] * 0x100 + bufferCNTR[2]*0x10000 + bufferCNTR[3] * 0x1000000;
        //posCount |= bufferCNTR[0] & BIT1 | bufferCNTR[1] & (BIT1 << 8);
        //posCount |= bufferCNTR[2] & (BIT1 << 16) | bufferCNTR[3] & (BIT1 << 24);
        posCount = bufferCNTR;

        break;

    case 2:// count = 150ms
        break;

    case 3:// count = 200ms
        break;

    case 4:// count = 250ms
        // every 250ms, LCDDisplay updated...
        //nokLcdSetPixel(xTest, yTest);

        break;
    }
    count++;
    if (count == 5){
        count = 0;
    }

    /*
    xTest++;
    if (xTest >= 80){
        xTest = 0;
        yTest++;
    }
    if (yTest >= 45){
        xTest = 0;
        yTest = 0;
        nokLcdClear();
    }
    */
    TA0CTL &= ~TAIFG;
}


/*************************************************************************************
 * Function Name    : initTA0
 * Description      : To initialize timer A0 as Upmode
 *
 * Arguments        : (int, var) cycle is for M cycle to compare with CCR0
 * Return           : (void) N/A
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/
void initTA0(int cycle){
    TA0CTL = TASSEL_2 | ID_0 | MC_1 | TAIE; // Timer A0 Control Register
    // TA Select_2 means SMCLK = 1.08 MHz
    // ID_0 means divisor 1 ( = SMCLK)
    // MC_1 Up Mode
    // TAIE is interrupt Enable...

    TA0CCR0 = cycle - 1;
    // capture/control register 0 = M - 1, M is M cycles where the timer rolls over up to the number.

    TA0CTL &= ~TAIFG; // clear flag
}
