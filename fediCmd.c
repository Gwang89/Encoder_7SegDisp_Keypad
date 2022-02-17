/*
 * fediCmd.c
 *
 *  Created on: 2022. 2. 9.
 *      Author: GwangSik
 */

#include <MSP430.h>
#include "LS7366R.h"
#include "fediCmd.h"
#include "nokLcdDraw.h"
#include "nok5110LCD.h"
#include <stdio.h>
#include "ucsiUart.h"

static double angleTotal = 0;
static double angle360 = 0;

/************************************************************************************
* Function: fediHome
* - to initialize posCount with the initVal passed by the caller.
*   this function does not modify the current posCount value. instead,
*   another global variable posHome is updated and when we calculate the current posCount
*   we consider currentPosCount = posCount (from the encoder) - posHome.
*   because posHome is a new reference.
* argument: (long int) initVal - new reference posCount
* return: (char) 0 success, otherwise failed.
* Author: Gwang Sik Kim
* Date: Feb 9, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int fediHome(){
    int errIdx = -1;

    /*
    unsigned char bufTemp[4]; //
    bufTemp[3] = (unsigned char)((long int) initVal & 0xFF);
    bufTemp[2] = (unsigned char)((long int)(initVal & 0xFF00) >> 8);
    bufTemp[1] = (unsigned char)((long int)(initVal & 0xFF0000) >> 16);
    bufTemp[0] = (unsigned char)((long int)(initVal & 0xFF000000) >> 24);
    errIdx = LS7366Rwrite(DTR, bufTemp); // 4 byte?
    */


    bufferDTR[3] = (unsigned char)((long int) posHome & 0xFF);
    bufferDTR[2] = (unsigned char)((long int)(posHome & 0xFF00) >> 8);
    bufferDTR[1] = (unsigned char)((long int)(posHome & 0xFF0000) >> 16);
    bufferDTR[0] = (unsigned char)((long int)(posHome & 0xFF000000) >> 24);
    errIdx = LS7366Rwrite(DTR, bufferDTR); // 4 byte?

    if(errIdx == 0){ // success
        errIdx = LS7366Rload(CNTR);
    }
    else{
        // do nothing? invalid.
    }

    errIdx = fediRead(CNTR);
    //posCount =

    if (errIdx == 0){
        // initialize
        //posHome = initVal;
        // for example, if initVal is 2000, now when posCount = 2000, it is considered as 0.

        // clear display
        nokLcdClear();

        // zero angular displacement...
        if (dispMode == 1){
            fediDisp(DISP_SINGLE_LINE_MODE);
        }
        else if (dispMode == 2){
            fediDisp(DISP_PROGRESS_BAR_MODE);
        }

    }


    return errIdx;


}

/************************************************************************************
* Function: fediClr
* - this function does not modify any posCount value
*   except display mode. Only clears the LCD pixels and nothing is displayed.
* argument: (void) n/a
* return: (void) n/a
* Author: Gwang Sik Kim
* Date: Feb 9, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void fediClr(void){
    dispMode = 0; // no display
    nokLcdClear();

}


/************************************************************************************
* Function: fediRead
* - this function is to read LS7366R registers and display it.
* argument: (char) reg - the target register to read from LS7366R
* return: (char) errIdx  - 0 success and 1 invalid.
* Author: Gwang Sik Kim
* Date: Feb 9, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
char fediRead(char reg){
    char errIdx = 1;
    char dispMsg[MAX_DISP_MSG];
    long int* bufferCNTRptr;
    unsigned char bufferReg;
    if (reg==MDR0||reg==MDR1||reg==STR){
        errIdx = LS7366Rread(reg, &bufferReg);
        if(errIdx == 0){//success
            sprintf(dispMsg, "fediRead register value = %x in hex \r\n", bufferReg);
            ucsiA1UartTxString((unsigned char*)dispMsg);
        }
    }
    else if (reg == CNTR){
        errIdx = LS7366Rread(reg, bufferCNTR);
        if(errIdx == 0){
            bufferCNTRptr = bufferCNTR;
            sprintf(dispMsg, "fediRead register value = %ld in decimal \r\n", *bufferCNTRptr);
            ucsiA1UartTxString((unsigned char*)dispMsg);
            sprintf(dispMsg, "fediRead register value = %x in hexa \r\n", *bufferCNTRptr);
            ucsiA1UartTxString((unsigned char*)dispMsg);
        }
    }
    else if (reg == DTR){
        errIdx = LS7366Rread(reg, bufferDTR);
        if(errIdx == 0){
            bufferCNTRptr = bufferDTR;
            sprintf(dispMsg, "fediRead register value = %ld in decimal \r\n", *bufferCNTRptr);
            ucsiA1UartTxString((unsigned char*)dispMsg);
            sprintf(dispMsg, "fediRead register value = %x in hexa \r\n", *bufferCNTRptr);
            ucsiA1UartTxString((unsigned char*)dispMsg);
        }
    }
    return errIdx;
    // 0 success, 1 invalid.
}



/************************************************************************************
* Function: fediDisp
* - this function is to control what display mode is selected.
*   and that mode is to be passed to nokLcdDraw.c modules.
* argument: (char) mode - 1 for single line, 2 for progress bar mode
* return: (int) errIdx - -1 failed, 1 or 2 valid.
* Author: Gwang Sik Kim
* Date: Feb 9, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int fediDisp(char mode){
    int errIdx = -1;
    switch(mode){
    case DISP_SINGLE_LINE_MODE:
        nokLcdClear();
        dispMode = DISP_SINGLE_LINE_MODE;
        errIdx = DISP_SINGLE_LINE_MODE;
        break;
    case DISP_PROGRESS_BAR_MODE:
        nokLcdClear();
        dispMode = DISP_PROGRESS_BAR_MODE;
        errIdx = DISP_PROGRESS_BAR_MODE;
        break;
    default:
        //do nothing
        break;
    }

    return errIdx;
}


/************************************************************************************
* Function: fediFw
* - to display the current Revolution / angles / posCount / posHome on the console.
* argument: (void) n/a
* return: (void) n/a
* Author: Gwang Sik Kim
* Date: Feb 9, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void fediFw(void){
    //revolution and angle display...

    //long int posTemp = *posCount - posHome;

    //double* angleTotal;
    angleTotal = ((double)posCntFrHm) / POSCOUNT_PER_REV;// angleTotal is expressed as revolution...
    int rev = (int)angleTotal; // truncate --> revolutions in integer.
    //double* angle360;
    angle360 = (angleTotal - (double)rev) * 360; // multiplied  by 360 makes it's now converted to 360 degress system.

    char dispMsg[MAX_DISP_MSG];
    sprintf(dispMsg, "Rev : %d   angle : %lf \r\n", rev, angle360);
    ucsiA1UartTxString((unsigned char*)dispMsg);
    sprintf(dispMsg, "posCount : %ld  posHome : %ld  posCntFrHm : %ld \r\n", *posCount, posHome, posCntFrHm);
    ucsiA1UartTxString((unsigned char*)dispMsg);
}




