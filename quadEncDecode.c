/*
 * quadEncDecode.c
 *
 *  Created on: 2021. 11. 23.
 *      Author: GwangSik
 */


#include <msp430.h>
#include <quadEncDecode.h>


/*pseudo code...
1.    when the encoder triggers an interrupt on CHAB_PORT, ISR is called and call qEncDecode.
2.  This function can access global variable qEdecoder, which has channelState[]. (0 to 3)
3.  Current A and Current B are updated by CHAB_PORT, CHAB_PORT & BIT1 = State[3], CHAB_PORT & BIT 0 = State[2].
4.  Following the truth table,
if the output A and B are in ¡®FORWARD¡¯ direction, increment posCount.
if the output A and B are in ¡®REVERSE¡¯ direction, decrement posCount.
5.  After that, Current A and B are stored at Previous A and B. (Push those values)
State[1] = State[3], State[0] = State[2].
6.  End.
 *
 */
void qEncDecode(void){
    // read and update
    qEdecoder.channelState[CHA_CURR_IDX] = ((CHAB_PORT & CHA) >> 5); // P1.5
    qEdecoder.channelState[CHB_CURR_IDX] = ((CHAB_PORT & CHB) >> 4); // P1.4


    if (qEdecoder.channelState[CHB_PREV_IDX]^qEdecoder.channelState[CHA_CURR_IDX]){
        if (~(qEdecoder.channelState[CHA_PREV_IDX]^qEdecoder.channelState[CHB_CURR_IDX])){
            // Forward direction
            qEdecoder.posCount++;
        }
    }

    else if(qEdecoder.channelState[CHA_PREV_IDX]^qEdecoder.channelState[CHB_CURR_IDX]){
        if(~(qEdecoder.channelState[CHB_PREV_IDX]^qEdecoder.channelState[CHA_CURR_IDX])){
            // reverse direction
            qEdecoder.posCount--;
        }
    }

    // stop and impossible cases are ignored.

    //current value is stored at previous state.
    qEdecoder.channelState[CHA_PREV_IDX] =  qEdecoder.channelState[CHA_CURR_IDX];
    qEdecoder.channelState[CHB_PREV_IDX] =  qEdecoder.channelState[CHB_CURR_IDX];

}


/*************************************************************************************
 * Function Name    : initqEdecoder
 * Description      : to initialize global variable qEdecoder's members
 *
 * Arguments        :  N/A (global variable)
 * Return           : (void) N/A
 *
 * Created Date     : Taken care of by created data of the main function.
 * Last Modified    : Usually taken care of by revision control.
 *
 **************************************************************************************/
void initqEdecoder(void){
    //global variables initialized...
    qEdecoder.channelState[0] = 0;
    qEdecoder.channelState[1] = 0;
    qEdecoder.channelState[2] = 0;
    qEdecoder.channelState[3] = 0;
    qEdecoder.posCount = 0;

}
