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
#include <keypad.h>
#include <SevenSegDisplay.h>
#include <quadEncDecode.h>

// proto-types
//void countToAng(int count, int* angle);
void countToAng(int count);
void updateDispDecDigit(int* angle, struct SEVEN_SEG_DISP* display);

// main
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer


	struct SEVEN_SEG_DISP sevenSegDisp[3];
	//unsigned char sysState = 0x0;
	sysState = 0x0;
	//struct QUAD_ENC_DECODER qEdecoder;
	qEdecoder.channelState[0] = 0;
    qEdecoder.channelState[1] = 0;
    qEdecoder.channelState[2] = 0;
    qEdecoder.channelState[3] = 0;
	qEdecoder.posCount = 0;



    // configure the interrupts
    P2IES &= ~(BIT7 | BIT6 | BIT5 | BIT4);             // set edge detection to L to H
    P2IFG = 0;                  // clear interrupt flags by READING the register.
    P2IE |= (BIT7 | BIT6 | BIT5 | BIT4);               // enable P2.1 interrupt. Always do this last.

    P1IES &= ~(CHA | CHB);
    P1IFG = 0;
    P1IE |= (CHA | CHB);

    // enable the global interrupts with intrinsic function _bis_SR_register
    _bis_SR_register(GIE);


    angleWheel = 0;

    // DISP output Initialization
    sevenSegDispInit(&sevenSegDisp[0]);

    /*int j=0;
    for (j=0; j<1000; j++){
        qEdecoder.posCount++;
    }
    */
    //int flag = 0;
    // program start
    while(1){
        //if (flag == 1){
            //countToAng(qEdecoder.posCount, &angleWheel);
            //
        countToAng(qEdecoder.posCount);
        //}


        ;// connect the motor and check the posCount increasing/decreasing when motor is rotating.
    }


    /*
    int angleWheel;
    countToAng(qEdecoder.posCount, &angleWheel);
    updateDispDecDigit(&angleWheel, &sevenSegDisp[0]);


    if (sevenSegDisp[0].ctrl == 1){
        decTo7Seg(&sevenSegDisp[0]);
        dispRefresh(&sevenSegDisp[0], 0);
    }
    else if (sevenSegDisp[1].ctrl == 1){
        decTo7Seg(&sevenSegDisp[1]);
        dispRefresh(&sevenSegDisp[0], 1);
    }
    else if (sevenSegDisp[2].ctrl == 1){
        decTo7Seg(&sevenSegDisp[2]);
        dispRefresh(&sevenSegDisp[0], 2);
    }

    */

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


/*void countToAng(int count, int* angle){
    double angleDouble = (double) count / PULSE_CONVERSION;
    *angle = (int)angleDouble;

    //*angle = count / PULSE_CONVERSION;

}
*/

void countToAng(int count){
    //double angleDouble = (double) count * 360 / COUNT_PER_REV;
    //angleWheel = (int)angleDouble;

    double angleDouble = (double) count*360 / GEAR_RATIO / (4 * PULSE_PER_REV);
    //angleWheel = (int)angleDouble;

    //double angleDouble = (double) count / 5000 * 360;
    angleWheel = (int)((int)angleDouble)%360;


    //*angle = count / PULSE_CONVERSION;

}



void updateDispDecDigit(int* angle, struct SEVEN_SEG_DISP* display){

    if (*angle >= 0){
        //positive
        display[0].decDigit = *angle % 10;
        display[1].decDigit = (*angle /10 )%10;
        display[2].decDigit = (((*angle /100))%10);
    }
    else{
        //negative
        display[0].decDigit = *angle % 10 * (-1);
        display[1].decDigit = (*angle /10 )%10 *(-1);
        display[2].decDigit = (((*angle /100))%10) * (-1);
    }
}
