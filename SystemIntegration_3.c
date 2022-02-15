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
 * Date Created : November 28, 2021
 * Date Modified: November 28, 2021
 *
 **************************************************************************************/

//include
#include <msp430.h> 
#include "keypad.h"
#include "SevenSegDisplay.h"
#include "quadEncDecode.h"

#define SI_1_MACRO      0
#define SI_2_MACRO      0
#define SI_3_MACRO      1

// proto-types
void countToAng(int count);
void updateDispDecDigit(struct SEVEN_SEG_DISP* display);
void initTA0(int cycle);

// global variables...
unsigned char sysState = 0x0;                               //extern from SevenSegDisplay.h
struct QUAD_ENC_DECODER qEdecoder = { {0,0,0,0}, 0};        //extern from quadEncDecode.h
int angleWheel = 0x0;                                       //extern from quadEncDecode.h
unsigned char debounceFlag = 0;
struct KEYPAD_4X4 myKeyPad; // keypad variable
struct SEVEN_SEG_DISP sevenSegDisp[3];
unsigned char keyPressed = 0;

// main
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer


    // DISP output Initialization
    sevenSegDispInit(sevenSegDisp);
	//struct QUAD_ENC_DECODER qEdecoder initialization...
	initqEdecoder();
	//
	keypadInit();


    // I/O PORT CONFIGURATION
    ROW_DIR &= ~(BIT7+BIT6+BIT5+BIT4);  // ROW = P2IN <7:4> -> ROW<4:1>
    ROW_REN = 0xF0; // pullup/down enabled
    ROW_PULL_DN = 0x0; // pull-down selected
    COL_DIR = (BIT2 | BIT1 | BIT0);     // COL = P6OUT <2:0> -> COL<3:1>

    COL = (COL1_ON | COL2_ON | COL3_ON);

    // configure the interrupts PORT 2
    P2IES &= ~(BIT7 | BIT6 | BIT5 | BIT4);             // set edge detection to L to H
    P2IFG = 0;                  // clear interrupt flags by READING the register.
    P2IE |= (BIT7 | BIT6 | BIT5 | BIT4);               // enable P2.1 interrupt. Always do this last.

    // encoder configuration for interrupts PORT 1
    P1IES &= ~(CHA | CHB);
    P1IFG = 0;
    if (MACRO_WO_ENC == 0){

        P1IE |= (CHA | CHB);
    }


    if ((SI_2_MACRO) || (SI_3_MACRO)){
        // timer configuration ...
        initTA0(PERIOD_DIGIT);

    }

    //_bis_SR_register(GIE);    // enable the global interrupts with intrinsic function _bis_SR_register
    _enable_interrupts();


	// program start

    int testDigitCtrl = 0;
    int testDigitTrig = 0;
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
    else if (SI_1_MACRO){
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
    else if (SI_2_MACRO){
        // timer is implemented...
        if (MACRO_WO_ENC){
            while(1){
                qEdecoder.posCount += COUNT_PER_DEG;// 1 deg?

                countToAng(qEdecoder.posCount);         // angleWheel updated
                updateDispDecDigit(sevenSegDisp);       // Disp array's DecDigit are updated...
            }
        }
        else{
            while(1){
                // do nothing
            }
        }
    }
    else if (SI_3_MACRO){
        // timer is implemented
        // key-pad is integrated...

        if (MACRO_WO_ENC){
            while(1){

                if(debounceFlag == 1){
                    P2IE &= ~(BIT7 | BIT6 | BIT5 | BIT4); // turn off the interrupts enables...
                    //COL |= (COL1_ON | COL2_ON | COL3_ON);
                    // debouncing pressing / releasing signal
                    __delay_cycles((long int) 3 * TEN_MILI_SECONDS);
                    P2IE |= (BIT7 | BIT6 | BIT5 | BIT4); // turn on the interrupt enables...


                    debounceFlag = 0;


                }

                if (keyPressed == 1){
                    myKeyPad.keyPressCnt++;
                    decodeKeyCoord();
                    keyBufferUpdate();
                    keyPressed = 2;
                }


                qEdecoder.posCount += COUNT_PER_DEG;// 1 deg?
                countToAng(qEdecoder.posCount);         // angleWheel updated
                updateDispDecDigit(sevenSegDisp);       // Disp array's DecDigit are updated...
            }
        }
        else{
            while(1){
                // how to test it?
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

    // this ISR starts when the timer counts up to period-digit, which is meant to be 6.7ms

    switch(count){
    case 0:// digit 1 case
        decTo7Seg(&sevenSegDisp[DIGIT1_IDX]);
        dispRefresh(sevenSegDisp, DIGIT1_IDX);
        break;

    case 1:// digit 2 case
        decTo7Seg(&sevenSegDisp[DIGIT2_IDX]);
        dispRefresh(sevenSegDisp, DIGIT2_IDX);
        break;

    case 2:// digit 3 case
        decTo7Seg(&sevenSegDisp[DIGIT3_IDX]);
        dispRefresh(sevenSegDisp, DIGIT3_IDX);

        break;
    }
    count++;
    if (count == 3){
        count = 0;
    }
    TA0CTL &= ~TAIFG;
}


/*************************************************************************************
 * ISR Name         : qEncRotate
 * Description      : interrupted by the encoder's output A and B
 *                    to calculate count pulses by the quadrature
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/
#pragma vector = PORT1_VECTOR
__interrupt void qEncRotate(void){
    switch(P1IV){
    case 0xA:
        // P1.4 -> output B is triggered
        qEncDecode();
        P1IES ^= (CHB);
        break;
    case 0xC:
        // P1.5 -> output A is triggered
        qEncDecode();
        P1IES ^= (CHA);
        break;
    }

    // after reading PxIV, PxIFG and PxIV are cleared. (each BIT)


}


/*************************************************************************************
 * ISR Name         : keypadPressed
 * Description      : interrupted by the keyPad to set the system State
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/
#pragma vector = PORT2_VECTOR
__interrupt void keypadPressed (void){



    if (keyPressed == 0){
        switch(P2IV){
        case 0xA: // P2.4 -> ROW1

            // key scan algorithm
            COL = COL1_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW1_DETECT){ // key 1 is detected
                myKeyPad.currentKeyCoord = 0x11;
            }
            COL = COL2_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW1_DETECT){ // key 2 is detected
                myKeyPad.currentKeyCoord = 0x12;
            }
            COL = COL3_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW1_DETECT){ // key 3 is detected
                myKeyPad.currentKeyCoord = 0x14;
            }
            COL = COL1_ON | COL2_ON | COL3_ON;
            //COL &= ~(COL1_ON | COL2_ON | COL3_ON);
            debounceFlag = 1;
            P2IES ^= (BIT4);


            break;

        case 0xC: // P2.5 -> ROW2
            // key scan algorithm
            COL = COL1_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW2_DETECT){ // key 4 is detected
                myKeyPad.currentKeyCoord = 0x21;
            }
            COL = COL2_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW2_DETECT){ // key 5 is detected
                myKeyPad.currentKeyCoord = 0x22;
            }
            COL = COL3_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW2_DETECT){ // key 6 is detected
                myKeyPad.currentKeyCoord = 0x24;
            }
            COL = COL1_ON | COL2_ON | COL3_ON;
            //COL &= ~(COL1_ON | COL2_ON | COL3_ON);
            debounceFlag = 1;
            P2IES ^= (BIT5);
            break;

        case 0xE: // P2.6 -> ROW3
            // key scan algorithm
            COL = COL1_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW3_DETECT){ // key 7 is detected
                myKeyPad.currentKeyCoord = 0x41;
            }
            COL = COL2_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW3_DETECT){ // key 8 is detected
                myKeyPad.currentKeyCoord = 0x42;
            }
            COL = COL3_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW3_DETECT){ // key 9 is detected
                myKeyPad.currentKeyCoord = 0x44;
            }
            COL = COL1_ON | COL2_ON | COL3_ON;
            //COL &= ~(COL1_ON | COL2_ON | COL3_ON);
            debounceFlag = 1;
            P2IES ^= (BIT6);
            break;

        case 0x10: // P2.7 -> ROW4

            // key scan algorithm
            COL = COL1_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW4_DETECT){ // key * is detected
                myKeyPad.currentKeyCoord = 0x81;
            }
            COL = COL2_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW4_DETECT){ // key 0 is detected
                myKeyPad.currentKeyCoord = 0x82;
            }
            COL = COL3_ON;
            if ((ROW& ROW_MASK) >> 4 == ROW4_DETECT){ // key # is detected
                myKeyPad.currentKeyCoord = 0x84;
            }
            COL = COL1_ON | COL2_ON | COL3_ON;
            //COL &= ~(COL1_ON | COL2_ON | COL3_ON);
            debounceFlag = 1;

            P2IES ^= (BIT7);
            break;
        }

        keyPressed = 1;


    }
    else if (keyPressed == 2){
        debounceFlag = 1;
        switch(P2IV){
        case 0xA:
            P2IES ^= (BIT4);
            break;
        case 0xC:
            P2IES ^= (BIT5);
            break;
        case 0xE:
            P2IES ^= (BIT6);
            break;
        case 0x10:
            P2IES ^= (BIT7);
            break;
        }
        keyPressed = 0;

    }


    P2IFG = 0;

}

/*************************************************************************************
 * Function Name    : countToAng
 * Description      : To convert the count to angle
 *
 * Arguments        : (int) count obtained by quadrature
 * Return           : (void) N/A --> global variable angleWheel is updated
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/
void countToAng(int count){
    double angleDouble = (double) count*DEG_PER_REV / GEAR_RATIO / (QUADRATURE * PULSE_PER_REV);
    angleWheel = (int)((int)angleDouble) % DEG_PER_REV;
}


/*************************************************************************************
 * Function Name    : updateDispDecDigit
 * Description      : To update each digit decoded from the angle value
 *
 * Arguments        : (SEVEN_SEG_DISP, pointer) display
 * Return           : (void) N/A
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/
void updateDispDecDigit(struct SEVEN_SEG_DISP* display){

    if (angleWheel >= 0){
        //positive
        display[DIGIT1_IDX].decDigit = angleWheel % 10;
        display[DIGIT2_IDX].decDigit = (angleWheel /10 )%10;
        display[DIGIT3_IDX].decDigit = (((angleWheel /100))%10);
    }
    else{
        //negative
        display[DIGIT1_IDX].decDigit = angleWheel % 10 * (-1);
        display[DIGIT2_IDX].decDigit = (angleWheel / 10 )%10 *(-1);
        display[DIGIT3_IDX].decDigit = (((angleWheel / 100))%10) * (-1);
    }
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
