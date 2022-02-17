#include <msp430.h> 
#include "nok5110LCD.h"
#include "usciSpi.h"
#include "ucsiUart.h"
#include <string.h>
#include <stdio.h>
#include "LS7366R.h"
#include "usciCmdInterpreter.h"
#include "fediCmd.h"

#define     DELAY_1SEC      1000000
#define     DELAY_100mSEC   100000
#define     CYCLE_COUNT 50000
//#define     CYCLE_COUNT 60000
#define     TEST1       0
#define     TEST2       0
#define     TEST3       1


// prototype
void initTA0(int cycle);

unsigned int xTest = 0, yTest = 0;
long int posCount;
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
    //char spiTxBuffer[50];
    //int bufLength;
    // ======================== =========== =================================


    CMD cmdList[MAX_CMDS];
    initCmdList(cmdList);

    //UCB1IE |= UCRXIE;
    __enable_interrupt();
	nokLcdInit();
	nokLcdClear();

	// start
	nokLcdLoading();
	//nokLcdDrawScrnLine(10, 0x0);
    //nokLcdDrawScrnLine(11, 0x0);
    //nokLcdDrawScrnLine(12, 0x0);

	__delay_cycles(DELAY_1SEC);
	nokLcdClear();

    // ======================== Timer config =================================

	unsigned int cycle = CYCLE_COUNT; // 1us -> 50000 count -> 50ms.
	initTA0(cycle);

    // ======================== ============ =================================

	int k=0;
	for (k=0; k<BUFF_SIZE; k++){
	    rxBuffer[k] = 0;
	}

	char* errorFlag;
	char dispMsg[40];
	//char lineNum[40];
	//char rxString[50];
	//unsigned int p= 0;

	int index = -2;
	int errIdx = -2;

    unsigned char xPos = 0, yPos = 0;
    //unsigned char testByte = 0x0;
    unsigned char p=0, q=0;

    int posCountTemp1 = 500;
    int posCountTemp2 = 500;
    while(1){

        if (TEST1){
            // single line is rotating...
            nokLcdClear();
            nokLcdDispLineMode(posCountTemp1);
            __delay_cycles(DELAY_100mSEC);
            posCountTemp1 -= 100;

        }

        if (TEST2){
            // single line is rotating...
            nokLcdClear();
            nokLcdDispLineMode(posCountTemp2);
            __delay_cycles(DELAY_100mSEC);
            posCountTemp2 += 100;
        }

        /*
        // toggle LED
        P4DIR |= BIT7;
        __delay_cycles(DELAY_1SEC); // 1sec delay
        P4OUT ^= BIT7;
        __delay_cycles(DELAY_1SEC); // 1sec delay
        P4OUT ^= BIT7;
        */


        // test blinking...
        /*
        while(1){
            nokLcdWrite(LCD_ALL_SEG_DISP,DC_CMD);
            __delay_cycles(1000000);
            nokLcdWrite(LCD_NORMAL_DISP,DC_CMD);
            __delay_cycles(1000000);

        }
        */


        // test set pixel..
        /*
        while(1){
            nokLcdClear();
            for (q=0; q<LCD_MAX_ROW - 1; q++){ // for some reason, y= 47, the line couldn't continue and crashed?
                for (p=0; p<LCD_MAX_COL; p++){
                    nokLcdSetPixel(xPos + p, yPos + q);
                    __delay_cycles(10000);
                }
            }
        }

        */


        if(TEST3){
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
