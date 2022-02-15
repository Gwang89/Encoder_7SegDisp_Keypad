/*************************************************************************************
 * ROBT 3356 Controller Systems
 * Lab 10 - Measuring and Displaying Angular Displacement in Real-Time
 *
 * Purpose      : To read the angle displacement by manual rotation of flywheel
 *                and to show what angle is by 3-digit 7-segment display
 *
 * Description  : This program is to get Quadrature output A and B from the motor Encoder
 *                and calculate the angle displacement and show the value through 3-digit
 *                7-segment displays.
 *
 *
 * Author       : Gwang Sik Kim, A01224567, Set ROBT3A
 *
 * Declaration  : I, Gwang Sik Kim, declare that the following program was written by me.
 *
 * Date Created : November 23, 2021
 * Date Modified: November 23, 2021
 *
 **************************************************************************************/

//include
#include <msp430.h> 
#include "keypad.h"
#include "SevenSegDisplay.h"
#include "quadEncDecode.h"

// proto-types
void countToAng(int count);
void updateDispDecDigit(struct SEVEN_SEG_DISP* display);

// global variables...
unsigned char sysState = 0x0;                               //extern from SevenSegDisplay.h
struct QUAD_ENC_DECODER qEdecoder = { {0,0,0,0}, 0};        //extern from quadEncDecode.h
int angleWheel = 0x0;                                       //extern from quadEncDecode.h
int keyISRFlagSet = 0;
struct KEYPAD_4X4 myKeyPad; // keypad variable

// main
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	struct SEVEN_SEG_DISP sevenSegDisp[3];

	//struct QUAD_ENC_DECODER qEdecoder;
	qEdecoder.channelState[0] = 0;
    qEdecoder.channelState[1] = 0;
    qEdecoder.channelState[2] = 0;
    qEdecoder.channelState[3] = 0;
	qEdecoder.posCount = 0;

	/*
    // configure the interrupts
    P2IES &= ~(BIT7 | BIT6 | BIT5 | BIT4);             // set edge detection to L to H
    P2IFG = 0;                  // clear interrupt flags by READING the register.
    P2IE |= (BIT7 | BIT6 | BIT5 | BIT4);               // enable P2.1 interrupt. Always do this last.
    */
    P1IES &= ~(CHA | CHB);
    P1IFG = 0;
    P1IE |= (CHA | CHB);

    // enable the global interrupts with intrinsic function _bis_SR_register
    _bis_SR_register(GIE);

	// program start
    // DISP output Initialization
    sevenSegDispInit(sevenSegDisp);




    int testDigitCtrl = 0;
    int testDigitTrig = 0;
    //int ctrlRep = 0; // 0 to 2 repetition
    int j=0, k=0;


    if (MACRO1){
        while(1){
            // each segment is turned on at a time, segment by segment...
            DIGIT |= (DISP1 | DISP2 | DISP3); //turn off all digits
            SEG = 0xFF; // initialize SEG
            if (k==0){
                DIGIT &= ~(DISP1); // anode control for digit 1
            }
            else if (k==1){
                DIGIT &= ~(DISP2); // anode control for digit 1
            }
            else if (k==2){
                DIGIT &= ~(DISP3); // anode control for digit 1
            }
            for (j=0; j<8 ;j++){
                SEG &= ~(BIT0 << j);
                __delay_cycles(INTERVAL);
            }
            k++;
            if (k==3){
                k=0;
            }
        }
    }


    else if (MACRO2){
        while(1){
            // each digit is turned on from 0 to 9 by INTERVAL delay...
            sevenSegDisp[DIGIT1_IDX].decDigit = testDigitTrig;
            sevenSegDisp[DIGIT2_IDX].decDigit = testDigitTrig;
            sevenSegDisp[DIGIT3_IDX].decDigit = testDigitTrig;

            decTo7Seg(&sevenSegDisp[DIGIT1_IDX]);
            dispRefresh(sevenSegDisp, DIGIT1_IDX);
            __delay_cycles(INTERVAL);

            decTo7Seg(&sevenSegDisp[DIGIT2_IDX]);
            dispRefresh(sevenSegDisp, DIGIT2_IDX);
            __delay_cycles(INTERVAL);

            decTo7Seg(&sevenSegDisp[DIGIT3_IDX]);
            dispRefresh(sevenSegDisp, DIGIT3_IDX);
            __delay_cycles(INTERVAL);

            testDigitCtrl++;
            if(testDigitCtrl == 10){
                testDigitTrig++;
                testDigitCtrl = 0;
            }
            //__delay_cycles(INTERVAL_1SEC);


            if (testDigitTrig == 10){
                testDigitTrig = 0;
            }
        }
    }

    // normal test mode...
    else{
        while(1){
            // continuously convert the posCount to Angle
            // and update the segment one digit at a time...
            // make sure the frequency is 50Hz...

            if (MACRO_WO_ENC){
                qEdecoder.posCount += COUNT_PER_DEG;// 1 deg?
            }
            countToAng(qEdecoder.posCount);         // angleWheel updated
            updateDispDecDigit(sevenSegDisp);       // Disp array's DecDigit are updated...

            decTo7Seg(&sevenSegDisp[DIGIT1_IDX]);
            dispRefresh(sevenSegDisp, DIGIT1_IDX);
            __delay_cycles(PERIOD_DIGIT);

            decTo7Seg(&sevenSegDisp[DIGIT2_IDX]);
            dispRefresh(sevenSegDisp, DIGIT2_IDX);
            __delay_cycles(PERIOD_DIGIT);

            decTo7Seg(&sevenSegDisp[DIGIT3_IDX]);
            dispRefresh(sevenSegDisp, DIGIT3_IDX);
            __delay_cycles(PERIOD_DIGIT);


        }

    }


	//while(1){


	    /*
	    switch(sysState){
	    case PWR:
            break;

	    case CLR:
            break;

	    case REFRESH:
            break;

	    case FLASH:
            break;

	    case KEY_PRESS:
            break;

	    default:
	        break;

	    }
	    */



	//}
	return 0;
}




#pragma vector = PORT1_VECTOR
__interrupt void qEncRotate(void){
    switch(P1IV){
    case 0xA:
        // P1.0 -> output B is triggered
        qEncDecode();
        P1IES ^= (CHB);
        break;
    case 0xC:
        // P1.1 -> output A is triggered
        qEncDecode();
        P1IES ^= (CHA);
        break;
    }

    // after reading PxIV, PxIFG and PxIV are cleared. (each BIT)


}



#pragma vector = PORT2_VECTOR
__interrupt void sysStateUpdate (void){
    switch(P2IV){
    case 0xA: // P2.4
        sysState = KEY_PRESS;
        break;

    case 0xC: // P2.5
        sysState = KEY_PRESS;
        break;

    case 0xE: // P2.6
        sysState = KEY_PRESS;
        break;

    case 0x10: // P2.7 -> ROW4

        // key scan algorithm
        COL = COL1_ON;
        if ((ROW& ROW_MASK) >> 4 == ROW4_DETECT){ // key * is detected
            sysState = PWR;
        }
        COL = COL2_ON;
        if ((ROW& ROW_MASK) >> 4 == ROW4_DETECT){ // key 0 is detected
            sysState = CLR;
        }
        COL = COL3_ON;
        if ((ROW& ROW_MASK) >> 4 == ROW4_DETECT){ // key # is detected
            sysState = FLASH;
        }
        COL = COL1_ON | COL2_ON | COL3_ON;
        break;
    }

    //P2IFG = 0;

}


void countToAng(int count){
    double angleDouble = (double) count*DEG_PER_REV / GEAR_RATIO / (QUADRATURE * PULSE_PER_REV);
    angleWheel = (int)((int)angleDouble) % DEG_PER_REV;
}



void updateDispDecDigit(struct SEVEN_SEG_DISP* display){

    if (angleWheel >= 0){
        //positive
        display[0].decDigit = angleWheel % 10;
        display[1].decDigit = (angleWheel /10 )%10;
        display[2].decDigit = (((angleWheel /100))%10);
    }
    else{
        //negative
        display[0].decDigit = angleWheel % 10 * (-1);
        display[1].decDigit = (angleWheel / 10 )%10 *(-1);
        display[2].decDigit = (((angleWheel / 100))%10) * (-1);
    }
}



