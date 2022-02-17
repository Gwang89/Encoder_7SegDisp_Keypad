#include <msp430.h> 
#include "nok5110LCD.h"
#include "usciSpi.h"
#include "ucsiUart.h"
#include <string.h>
#include <stdio.h>
#include "LS7366R.h"
#include "usciCmdInterpreter.h"
#include "fediCmd.h"
#include "nokLcdDraw.h"


#define     DELAY_1SEC  1000000
#define     CYCLE_COUNT 50000
//#define     CYCLE_COUNT 60000


// demo macro
#define     TEST1           0
#define     TEST2           0
#define     TEST3           1
#define     TEST_HOME       0

#define     COUNT1          0   //positive, increasing
#define     COUNT2          0   //positive, decreasing
#define     COUNT3          0   //negative, increasing
#define     COUNT4          0   //negative, decreasing

// prototype
void initTA0(int cycle);

//global
//long signed int posCount;
long signed int* posCount;
long signed int posCountPrev;
unsigned char bufferCNTR[4] = {0, 0, 0, 0};
unsigned char bufferDTR[4] = {0, 0, 0, 0};
long signed int posHome;
char dispMode = 0;
char testMode = 1;
long signed int posCountTest;
long int posCntFrHm;

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

    // ======================== Timer config =================================

    unsigned int cycle = CYCLE_COUNT; // 1us -> 50000 count -> 50ms.
    initTA0(cycle);

    // ======================== ============ =================================

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

    //__disable_interrupt();

	dispMode = 0;
    //__enable_interrupt();

    // initialize posHome to posCount
    posHome = 0;
    posCountPrev = 0;



    // ======================== LS7366R init  =================================

	LS7366Rinit();

    // ======================== ============ =================================


    //int i;
    char* errorFlag;
    char dispMsg[40];
    int index = -2;
    int errIdx = -2;

    if(TEST_HOME){
        posCountTest = 0;
        posHome = 0;
    }

    if(COUNT1){
        posCountTest = 2000;
    }
    else if(COUNT2){
        posCountTest = 84000;
    }
    else if(COUNT3){
        posCountTest = -82000;
    }
    else if(COUNT4){
        posCountTest = -2000;
    }

    initPrev();






    while(1){

        if(TEST2){

            LS7366Rclear(CNTR);
            //posCount = 0;
            while(1){
                //sprintf(dispMsg, "%ld\n", posCount);
                // modifiers are not allowed for minimul printf mode?
                // see the settings and support
                sprintf(dispMsg, "%ld\n", *posCount);
                ucsiA1UartTxString((unsigned char*)dispMsg);


            }

        }



        else if(TEST3){
            LS7366Rclear(CNTR);
            while(1){
                // initial display - waiting for the command
                //sprintf(dispMsg, "Waiting for the user...\n");
                sprintf(dispMsg, "Waiting for the user...\r\n");
                ucsiA1UartTxString((unsigned char*)dispMsg);
                rewind(stdin);
                errorFlag = usciA1UartGets(rxString);
                rewind(stdin);
                // checking if it's NULL
                if (errorFlag[0] == ASCII_NULL){
                    sprintf(dispMsg, "NULL cannot be accepted.\r\n");
                    ucsiA1UartTxString((unsigned char*)dispMsg);
                }
                else{
                    // checking if it's valid command
                    sprintf(dispMsg, "User Enters a command... \r\n");
                    ucsiA1UartTxString((unsigned char*)dispMsg);

                    // checking the index from the command list
                    index = parseCmd(cmdList, rxString);
                    sprintf(dispMsg, "The command index is %d.\r\n", index);
                    ucsiA1UartTxString((unsigned char*)dispMsg);

                    // execute command
                    errIdx = executeCmd(cmdList, index);
                    if(errIdx == -1){ // failure case
                        sprintf(dispMsg, "The command arguments were not acceptable.\r\n");
                        ucsiA1UartTxString((unsigned char*)dispMsg);
                    }
                    else{// success case
                        sprintf(dispMsg, "Successful.\r\n");
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
        if(TEST_HOME){
            //posCount += 30;
            if(testMode == 1){
                posCountTest += 460;
            }
            else if(testMode == 2){
                posCountTest -= 460;
            }
            //posCount += 460;

        }
        else if(COUNT1){
            posCountTest +=460;
        }
        else if(COUNT2){
            posCountTest -= 460;
        }
        else if(COUNT3){
            posCountTest += 460;
        }
        else if(COUNT4){
            posCountTest -= 460;
        }
        else{
            //*posCount = bufferCNTR
            LS7366Rread(CNTR, bufferCNTR); // read CNTR and update it.


            //posCount = bufferCNTR[0] * 0x1 + bufferCNTR[1] * 0x100 + bufferCNTR[2]*0x10000 + bufferCNTR[3] * 0x1000000;
            posCount = bufferCNTR;

            //posCount |= bufferCNTR[0] & BIT1 | bufferCNTR[1] & (BIT1 << 8);
            //posCount |= bufferCNTR[2] & (BIT1 << 16) | bufferCNTR[3] & (BIT1 << 24);
            //posCountTest = bufferCNTR;

            //posCount = bufferCNTR;

        }


        break;

    case 2:// count = 150ms
        break;

    case 3:// count = 200ms
        break;

    case 4:// count = 250ms
        // every 250ms, LCDDisplay updated...
        //nokLcdSetPixel(xTest, yTest);
        posCntFrHm = *posCount - posHome;
        if(dispMode == 1){
            if (*posCount - posHome != posCountPrev){
                nokLcdDispLineMode();
                // posHome is the reference point... pass the difference.
            }


        }
        else if(dispMode == 2){
            if (*posCount - posHome != posCountPrev){
                nokLcdDispBarMode();
                // posHome is the reference point... pass the difference.
            }
        }

        break;
    }
    count++;
    if (count == 5){
        count = 0;
    }

    if (posCountTest > 46000){
        testMode = 2;
    }

    if(posCountTest < -46000){
        testMode = 1;
    }


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
