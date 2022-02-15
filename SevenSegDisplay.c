/*
 * SevenSegDisplay.c
 *
 *  Created on: 2021. 11. 23.
 *      Author: GwangSik
 */




#include <keypad.h>
#include <msp430.h>
#include <SevenSegDisplay.h>


// function definition or implementation file
/*************************************************************************************
 * Function Name    : sevenSegDispInit
 * Description      : To initialize SEVEN_SEG_DISP
 *
 * Arguments        : display (pointer, SEVEN_SEG_DISP*)
 * Return           : (void) N/A
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/

void sevenSegDispInit(struct SEVEN_SEG_DISP* display){

    //initialization
    int i;
    for (i = 0; i<NUM_DISP ; i++){
        //components are all zero
        display[i].decDigit = 0;
        display[i].binSegCode = 0x0;
        display[i].ctrl = 0;

    }


    // DIGIT and SEG output config
    DIGIT_DIR |= BIT0 | BIT1 | BIT2; // P4.0, P4.1, P4.2  = 1, Output
    SEG_DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7; // P3<7:0> as outputs

    // turn off
    SEG |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7; // active low. 1 means 'LED off'
    DIGIT |= DISP1 | DISP2 | DISP3; // active low. 1 means 'display off'
}


/*************************************************************************************
 * Function Name    : decTo7Seg
 * Description      : To decode decimal to 7 segment
 *
 * Arguments        : display (pointer, SEVEN_SEG_DISP*)
 * Return           : (void) N/A
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/
void decTo7Seg(struct SEVEN_SEG_DISP* display){


    switch(display->decDigit){
    case 0:
        //SEG = 0x03;
        display->binSegCode |= SEG_g | SEG_DP;
        display->binSegCode &= ~(SEG_a | SEG_b | SEG_c | SEG_d | SEG_e | SEG_f);
        break;

    case 1:
        //SEG = 0x9F;
        display->binSegCode |= (SEG_a | SEG_d | SEG_e | SEG_f | SEG_g | SEG_DP);
        display->binSegCode &= ~(SEG_b | SEG_c);
        break;

    case 2:
        //SEG = 0x25;
        display->binSegCode |= ( SEG_c | SEG_f | SEG_DP);
        display->binSegCode &= ~( SEG_a | SEG_b | SEG_d | SEG_e | SEG_g );
        break;

    case 3:
        //SEG = 0x0D;
        display->binSegCode |= ( SEG_e | SEG_f | SEG_DP );
        display->binSegCode &= ~( SEG_a | SEG_b | SEG_c | SEG_d | SEG_g );
        break;

    case 4:
        // SEG = 0x99
        display->binSegCode |= ( SEG_a | SEG_d | SEG_e | SEG_DP );
        display->binSegCode &= ~( SEG_b | SEG_c | SEG_f | SEG_g);
        break;

    case 5:
        // SEG = 0x45
        display->binSegCode |= ( SEG_b | SEG_e | SEG_DP );
        display->binSegCode &= ~( SEG_a | SEG_c | SEG_d | SEG_f | SEG_g );
        break;

    case 6:
        // 0x41
        display->binSegCode |= ( SEG_b | SEG_DP );
        display->binSegCode &= ~( SEG_a | SEG_c | SEG_d | SEG_e | SEG_f | SEG_g );
        break;

    case 7:
        // SEG = 0x1F
        display->binSegCode |= ( SEG_d | SEG_e | SEG_f | SEG_g | SEG_DP );
        display->binSegCode &= ~( SEG_a | SEG_b | SEG_c );
        break;

    case 8:
        //SEG = 0x01;
        display->binSegCode |= ( SEG_DP );
        display->binSegCode &= ~( SEG_a | SEG_b | SEG_c | SEG_d | SEG_e | SEG_f | SEG_g );
        break;

    case 9:
        //SEG = 0x09;
        display->binSegCode |= ( SEG_e | SEG_DP );
        display->binSegCode &= ~( SEG_a | SEG_b | SEG_c | SEG_d | SEG_f | SEG_g);
        break;


    case -1:
        // SEG = 0x9E;
        display->binSegCode |= (SEG_a | SEG_d | SEG_e | SEG_f | SEG_g );
        display->binSegCode &= ~(SEG_b | SEG_c | SEG_DP);
        break;

    case -2:
        // SEG = 0x24
        display->binSegCode |= ( SEG_e | SEG_f );
        display->binSegCode &= ~( SEG_a | SEG_b | SEG_c | SEG_d | SEG_g  | SEG_DP );
        break;

    case -3:
        // SEG = 0x0C
        display->binSegCode |= ( SEG_e | SEG_f);
        display->binSegCode &= ~( SEG_a | SEG_b | SEG_c | SEG_d | SEG_g  | SEG_DP );
        break;

    case -4:
        // SEG = 0x98
        display->binSegCode |= ( SEG_a | SEG_d | SEG_e );
        display->binSegCode &= ~( SEG_b | SEG_c | SEG_f | SEG_g | SEG_DP );
        break;

    case -5:
        // SEG = 0x44
        display->binSegCode |= ( SEG_b | SEG_e);
        display->binSegCode &= ~( SEG_a | SEG_c | SEG_d | SEG_f | SEG_g  | SEG_DP );
        break;

    case -6:
        // SEG = 0x40
        display->binSegCode |= ( SEG_b );
        display->binSegCode &= ~( SEG_a | SEG_c | SEG_d | SEG_e | SEG_f | SEG_g | SEG_DP );
        break;

    case -7:
        // SEG = 0x1E
        display->binSegCode |= ( SEG_d | SEG_e | SEG_f | SEG_g  );
        display->binSegCode &= ~( SEG_a | SEG_b | SEG_c| SEG_DP );
        break;

    case -8:
        // SEG = 0x00
        //SEG |= ( );
        display->binSegCode &= ~( SEG_a | SEG_b | SEG_c | SEG_d | SEG_e | SEG_f | SEG_g | SEG_DP);
        break;

    case -9:
        // SEG = 0x08
        display->binSegCode |= ( SEG_e );
        display->binSegCode &= ~( SEG_a | SEG_b | SEG_c | SEG_d | SEG_f | SEG_g | SEG_DP);
        break;

    default:
        //E ? Error?
        display->binSegCode |= (SEG_b | SEG_c  | SEG_DP );
        display->binSegCode &= ~(SEG_a | SEG_d | SEG_e | SEG_f | SEG_g);
        break;

    }



}

/*************************************************************************************
 * Function Name    : dispRefresh
 * Description      : To fresh 7-segment displays using Index
 *                    index 0 = DIGIT 1
 *                    index 1 = DIGIT 2
 *                    index 2 = DIGIT 3
 *
 * Arguments        : display (pointer, SEVEN_SEG_DISP*)
 *                    dispIndex (variable, unsigned char)
 * Return           : (void) N/A
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/
void dispRefresh(struct SEVEN_SEG_DISP* display, unsigned char dispIndex){

    DIGIT |= (DISP1 | DISP2 | DISP3);
    switch(dispIndex){
    case 0:
        display[0].ctrl = 1;
        display[1].ctrl = 0;
        display[2].ctrl = 0;
        //decTo7Seg(&display[dispIndex]);

        SEG = display[dispIndex].binSegCode;
        DIGIT &= ~(DISP1);
        break;
    case 1:
        display[0].ctrl = 0;
        display[1].ctrl = 1;
        display[2].ctrl = 0;
        //decTo7Seg(&display[dispIndex]);

        SEG = display[dispIndex].binSegCode;
        DIGIT &= ~(DISP2);
        break;
    case 2:
        display[0].ctrl = 0;
        display[1].ctrl = 0;
        display[2].ctrl = 1;
        //decTo7Seg(&display[dispIndex]);

        SEG = display[dispIndex].binSegCode;
        DIGIT &= ~(DISP3);
        break;
    }

}
