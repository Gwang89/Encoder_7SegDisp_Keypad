/*
 * lab9_ISR.c
 *
 *  Created on: 2021. 11. 28.
 *      Author: GwangSik
 */




/*************************************************************************************
 * ROBT 3356 Controller Systems
 * Lab 9 - KeyPad Firmware with interrupts
 *
 * Purpose      : To implement KeyPad Input and LED Output
 *
 * Description  : This program is to get inputs using 4 * 3 KeyPad
 *                and to output 8 * 1 LED array
 *
 *
 * Author       : Gwang Sik Kim, A01224567, Set ROBT3A
 *
 * Declaration  : I, Gwang Sik Kim, declare that the following program was written by me.
 *
 * Date Created : November 16, 2021
 * Date Modified: November 16, 2021
 *
 **************************************************************************************/



/*************************************************************************************
 * functionality of the program;
 * getKeyPress : done
 * debouncing the button: done
 * getBuffer : done
 * keyPressCnt : done
 * currentKey : done
 *
 * decodeKeyCoord : done - note that the keyPad is 4 x 3
 * updateLedArray : done - note that the P3OUT <7:0> is used, so P3.3 is not being turned on
 *                         because column is up to 3, not 4.
 *
**************************************************************************************/


/*
 * interrupt is added. Nov. 16. 2021
 */
#include <msp430.h>
#include <keypad.h>



// main
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    //initialization
    ROW_DIR &= ~(BIT7+BIT6+BIT5+BIT4);  // ROW = P2IN <7:4> -> ROW<4:1>
    ROW_REN = 0xF0; // pullup/down enabled
    ROW_PULL_DN = 0x0; // pull-down selected

    COL_DIR = (BIT2 | BIT1 | BIT0);     // COL = P6OUT <2:0> -> COL<3:1>
    LED_DIR = (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);  // LED = P3OUT <7:0>

    COL = (COL1_ON | COL2_ON | COL3_ON);
    LED = 0x0; // output initialization


    // configure the interrupts
    P2IES &= ~(BIT7 | BIT6 | BIT5 | BIT4);             // set edge detection to L to H
    P2IFG = 0;                  // clear interrupt flags by READING the register.
    P2IE |= (BIT7 | BIT6 | BIT5 | BIT4);               // enable P2.1 interrupt. Always do this last.

    // enable the global interrupts with intrinsic function _bis_SR_register
    _bis_SR_register(GIE);

    int decodeValid;
 // --> why is it going back to -1?
    // program start
    keyISRFlagSet = -1;
    keypadInit(); // initialize the structure variable
    while(1){

        COL = (COL1_ON | COL2_ON | COL3_ON);
        //flag = getKeyPress(&myKeyPad);

        //if (keyISRFlagSet == 0){

        //}


        if (keyISRFlagSet == 0){

            // return from ISR
            // turn off the interrupt enable.
           P2IE &= ~(BIT7 | BIT6 | BIT5 | BIT4);

            // debouncing pressing signal
            __delay_cycles((long int) 5 * TEN_MILI_SECONDS);
            while ((ROW & ROW_MASK)){
            }// while pressing, hold it.
            __delay_cycles((long int) 5 * TEN_MILI_SECONDS);



            // return 0 means success. Key is pressed.
            myKeyPad.keyPressCnt++;
            decodeKeyCoord();
            keyBufferUpdate();

            // return 0 if valid.
            updateLedArray();
            __delay_cycles((long int) 50 * TEN_MILI_SECONDS);
            keyISRFlagSet = -1;

            /*
            if(decodeValid == 0){
                // return 0 if valid.
                updateLedArray(decodeValid);
                __delay_cycles((long int) 50 * TEN_MILI_SECONDS);
                keyISRFlagSet = -1;
            }
            else{
                // return -1 if invalid
                LED = 0xFF;
                keyISRFlagSet = -1;
            }
            */

            // turn on the interrupt enable.
           P2IE |= (BIT7 | BIT6 | BIT5 | BIT4);




        }
        else if (keyISRFlagSet == -1){
            // return -1 means failure. Key is not pressed. still waiting
            LED = 0x0;
            //keyISRFlagSet = -1;
        }
        else{
            // something is wrong.
            ///LED = 0xFF; // ERROR occured.
            //keyISRFlagSet = -1;
        }
    }

    return 0;
}




#pragma vector = PORT2_VECTOR
__interrupt void keypadPressed (void){

    // debouncing releasing switch

    //__delay_cycles((long int) 50*DELAY_1000);

    //keyISRFlagSet = 1;
    // scan algorithm

    // column 1 scan
    COL = COL1_ON;
    if ( (ROW& ROW_MASK) >> 4 ){
        switch( (ROW & ROW_MASK) >> 4 ){
        case ROW1_DETECT:
            myKeyPad.currentKeyCoord = 0x11;
            break;
        case ROW2_DETECT:
            myKeyPad.currentKeyCoord = 0x21;
            break;
        case ROW3_DETECT:
            myKeyPad.currentKeyCoord = 0x41;
            break;
        case ROW4_DETECT:
            myKeyPad.currentKeyCoord = 0x81;
            break;
        default:
            break;
        }
        keyISRFlagSet = 0; // stop
    }

    // column 2 scan
    COL = COL2_ON;
    if(keyISRFlagSet * ((ROW& ROW_MASK) >> 4)){
        switch((ROW & ROW_MASK)>>4 ){
        case ROW1_DETECT:
            myKeyPad.currentKeyCoord = 0x12;
            break;
        case ROW2_DETECT:
            myKeyPad.currentKeyCoord = 0x22;
            break;
        case ROW3_DETECT:
            myKeyPad.currentKeyCoord = 0x42;
            break;
        case ROW4_DETECT:
            myKeyPad.currentKeyCoord = 0x82;
            break;
        default:
            break;
        }
        keyISRFlagSet = 0;
    }

    // column 3 scan
    COL = COL3_ON;
    if (keyISRFlagSet *((ROW& ROW_MASK) >> 4)){
        switch((ROW & ROW_MASK)>>4 ){
        case ROW1_DETECT:
            myKeyPad.currentKeyCoord = 0x14;
            break;
        case ROW2_DETECT:
            myKeyPad.currentKeyCoord = 0x24;
            break;
        case ROW3_DETECT:
            myKeyPad.currentKeyCoord = 0x44;
            break;
        case ROW4_DETECT:
            myKeyPad.currentKeyCoord = 0x84;
            break;
        default:
            break;
        }
        keyISRFlagSet = 0;
    }
    //*/

    COL = (COL1_ON | COL2_ON | COL3_ON);
    //while((ROW & ROW_MASK) >> 4){
    //}; // the button is pressed once, this while loop will end when the button is 'released'
    //__delay_cycles( 3 * TEN_MILI_SECONDS); // debouncing



    //flag = getKeyPress(&myKeyPad);


    // clear flag
    P2IFG = 0;

}


