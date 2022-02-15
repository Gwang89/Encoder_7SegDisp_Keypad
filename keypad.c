/*
 * keypad.c
 *
 *  Created on: 2021. 11. 9.
 *      Author: GwangSik
 */



#include <keypad.h>
#include <msp430.h>


// function definition or implementation file
/*************************************************************************************
 * Function Name    : keypadInit
 * Description      : To initialize keypad
 *
 * Arguments        : keypad (pointer, KEYPAD_4X4)
 * Return           : (void) N/A
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/

void keypadInit(void){


    myKeyPad.currentKeyCoord = 0;
    myKeyPad.keyPressCnt = 0;
    int i;
    for (i=KEY_BUFF_SZ - 1; i>=0; i--){
        myKeyPad.keyBuffer[i] = 0;
    };

    myKeyPad.currentKey = 0;
    for (i=KEY_BUFF_SZ - 1; i>=0; i--){
        myKeyPad.keyBufferPad[i] = 0;
    };




}


/*************************************************************************************
 * Function Name    : getKeyPress
 * Description      : To detect a key press from the key pad and
 *                    updates the following members after the key is released
 *
 * Arguments        : keypad (pointer, KEYPAD_4X4)
 * Return           : (int) return 0 if a key was pressed and -1 if not.
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/

int getKeyPress(void){

    int pressed;
    int flag = 1;

    // ROW is the input port waiting for the signal.
    // if ROW is non-zero, any button is pressed.
    if((ROW & ROW_MASK) >> 4){ //
        pressed = 0;

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
            flag = 0; // stop
        }

        // column 2 scan
        COL = COL2_ON;
        if(flag * ((ROW& ROW_MASK) >> 4)){
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
            flag = 0;
        }

        // column 3 scan
        COL = COL3_ON;
        if (flag *((ROW& ROW_MASK) >> 4)){
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
            flag = 0;
        }
        //*/

        COL = (COL1_ON | COL2_ON | COL3_ON);
        //while((ROW & ROW_MASK) >> 4){
        //}; // the button is pressed once, this while loop will end when the button is 'released'
        //__delay_cycles( 3 * TEN_MILI_SECONDS); // debouncing

    }
    else{
        // do nothing.
        pressed = -1;
    }




    return pressed;
}


/*************************************************************************************
 * Function Name    : decodeKeyCoord
 * Description      : To decode the keyCoordinates to the corresponding key numbers and alphabets
 *
 * Arguments        : keypad (pointer, KEYPAD_4X4)
 * Return           : (int) return 0 if a key was pressed and -1 if not.
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/

int decodeKeyCoord(void){

    int valid;

    switch(myKeyPad.currentKeyCoord){
    case 0x11:
        myKeyPad.currentKey = 1;
        valid = 0;
        break;
    case 0x12:
        myKeyPad.currentKey = 2;
        valid = 0;
        break;
    case 0x14:
        myKeyPad.currentKey = 3;
        valid = 0;
        break;
    case 0x21:
        myKeyPad.currentKey = 4;
        valid = 0;
        break;
    case 0x22:
        myKeyPad.currentKey = 5;
        valid = 0;
        break;
    case 0x24:
        myKeyPad.currentKey = 6;
        valid = 0;
        break;
    case 0x41:
        myKeyPad.currentKey = 7;
        valid = 0;
        break;
    case 0x42:
        myKeyPad.currentKey = 8;
        valid = 0;
        break;
    case 0x44:
        myKeyPad.currentKey = 9;
        valid = 0;
        break;
    case 0x81:
        myKeyPad.currentKey = 42; // from ASCII, * is 42 in decimal
        valid = 0;
        break;
    case 0x82:
        myKeyPad.currentKey = 0;
        valid = 0;
        break;
    case 0x84:
        myKeyPad.currentKey = 35; // from ASCII, # is 35 in decimal
        valid = 0;
        break;
    default:
        valid = -1;

    }

    return valid;
}

/*************************************************************************************
 * Function Name    : updateLedArray
 * Description      : To update LED array referring to the pressed button
 *                    indicating ROW and COL number
 *                    and if error detected, turn on ALL LEDs.
 *
 * Arguments        : keypad (pointer, KEYPAD_4X4)
 * Return           : (void) N/A
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 **************************************************************************************/
void updateLedArray(void){

    LED = myKeyPad.currentKeyCoord;
    //if (decodeValid == 0){

    //}
    //else{
    //    LED = 0xFF;
    //}
}

/*************************************************************************************
 * Function Name    : keyBufferUpdate
 * Description      : To store the history of pressed buttons from currentKeyCoord
 *                    the array buffer stores up to the number of KEY_BUFF_SZ
 *
 *
 * Arguments        : keypad (pointer, KEYPAD_4X4)
 * Return           : (void) N/A
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 **************************************************************************************/
void keyBufferUpdate(void){
    //unsigned char temp =0;
    unsigned int i;
    for (i= KEY_BUFF_SZ - 1; i>=1; i--){
        myKeyPad.keyBuffer[i] = myKeyPad.keyBuffer[i - 1];
    }
    myKeyPad.keyBuffer[0] = myKeyPad.currentKeyCoord;

    for (i= KEY_BUFF_SZ - 1; i>=1; i--){
        myKeyPad.keyBufferPad[i] = myKeyPad.keyBufferPad[i - 1];
    }
    myKeyPad.keyBufferPad[0] = myKeyPad.currentKey;


}

