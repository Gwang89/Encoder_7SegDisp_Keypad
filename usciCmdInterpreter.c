/*************************************************************************************************
 * ucsiUart.c
 * - C implementation or source file for MSP430 UCSI UART A1
 *
 *  Author: Greg Scutt
 *  Created on: March 1, 2017
 *  Modified: February 26th, 2018
 **************************************************************************************************/

#include <msp430.h>
#include <stdio.h>
#include "ucsiUart.h"
#include <string.h>
#include <stdlib.h>
#include "usciSpi.h"
#include "nok5110LCD.h"
#include "usciCmdInterpreter.h"
#include "LS7366R.h"
#include "fediCmd.h"

/************************************************************************************
* Function: initCmdList
* - initialize cmds array structure with the pre-defined names and the number of arguments
*  also in case, initialize the argument arrays with zeros instead of potential garbage
* Arguments: (CMD*) cmds
*
* return: (void) nothing
* Author: Gwang Sik Kim
* Date: Jan 19, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void initCmdList(CMD* cmds){
    cmds[0].name = CMD0_NAME;
    cmds[1].name = CMD1_NAME;
    cmds[2].name = CMD2_NAME;
    cmds[3].name = CMD3_NAME;
    cmds[4].name = CMD4_NAME;
    cmds[5].name = CMD5_NAME;
    cmds[6].name = CMD6_NAME;
    cmds[7].name = CMD7_NAME;
    cmds[8].name = CMD8_NAME;
    cmds[9].name = CMD9_NAME;
    cmds[10].name = CMD10_NAME;
    cmds[11].name = CMD11_NAME;


    cmds[0].nArgs = CMD0_NARGS;
    cmds[1].nArgs = CMD1_NARGS;
    cmds[2].nArgs = CMD2_NARGS;
    cmds[3].nArgs = CMD3_NARGS;
    cmds[4].nArgs = CMD4_NARGS;
    cmds[5].nArgs = CMD5_NARGS;
    cmds[6].nArgs = CMD6_NARGS;
    cmds[7].nArgs = CMD7_NARGS;
    cmds[8].nArgs = CMD8_NARGS;
    cmds[9].nArgs = CMD9_NARGS;
    cmds[10].nArgs = CMD10_NARGS;
    cmds[11].nArgs = CMD11_NARGS;



    unsigned int i=0, j=0;
    for (i = 0; i<MAX_CMDS; i++){
        for (j=0; j<MAX_ARGS; j++){
            cmds[i].args[j] = 0;
        }
    }
}


/************************************************************************************
* Function: parseCmd
* - parse the input cmdLine by delimiters (space, comma, tap) and store the tokens into
*   corresponding cmdList's arguments if the command is correct. return -1 if it fails.
* Arguments: (CMD*) cmdList  - array structure with pre-initialized values
*            (char*) cmdLine - input from the terminal
*
* return: 0 if succeeded, -1 if failed.
* Author: Gwang Sik Kim
* Date: Jan 19, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int parseCmd(CMD* cmdList, char* cmdLine){
    char* token[MAX_ARGS + 1];
    const char* deLim = " , ";

    //parsing - token0 = command name
    token[0] = strtok(cmdLine, deLim);
    unsigned int i=1;
    while(i<MAX_ARGS+1){
        token[i] = strtok(NULL,deLim);
        if (token[i] == NULL){
            break;
        }
        else{
            i++;
        }

    }
    int cmdIndex = -1;
    cmdIndex = validateCmd(cmdList, token[0]);
    // if it succeeds, it returns the index for the corresponding command.

    char dispErr[40];

    // token parsing continued. -> input arguments
    if (cmdIndex == 0 || cmdIndex == 1){
        cmdList[cmdIndex].args[0] = atoi(token[1]); // pNum
        cmdList[cmdIndex].args[1] = (int)strtol(token[2],NULL,16); // hexa number input!!
        cmdList[cmdIndex].args[2] = atoi(token[3]); // dir or state
    }
    else if(cmdIndex == 2){
        cmdList[cmdIndex].args[0] = (int)strtol(token[1],NULL,16); // byte
    }
    else if(cmdIndex == 3){//nokLcdSetPixel - x,y coordinates
        cmdList[cmdIndex].args[0] = atoi(token[1]); // x coordinate
        cmdList[cmdIndex].args[1] = atoi(token[2]); // y coordinate
    }
    else if(cmdIndex == 4){//nokLcdClear
        // do nothing. no argument.
    }
    else if(cmdIndex == 5){//nokLcdDrawScrnLine - position, hor/ver - 0x0 or 0x2
        cmdList[cmdIndex].args[0] = atoi(token[1]); // position
        cmdList[cmdIndex].args[1] = (int)strtol(token[2],NULL,16); // hexa number input!!
    }
    else if(cmdIndex == 6){//nokLcdDrawLine - x1, y1, x2, y2
        cmdList[cmdIndex].args[0] = atoi(token[1]); // x coordinate
        cmdList[cmdIndex].args[1] = atoi(token[2]); // y coordinate
        cmdList[cmdIndex].args[2] = atoi(token[3]); // x coordinate
        cmdList[cmdIndex].args[3] = atoi(token[4]); // y coordinate
    }
    else if(cmdIndex == 7){//fediHome
        cmdList[cmdIndex].args[0] = atoi(token[1]); // position
    }
    else if(cmdIndex == 8){//fediClr
        // do nothing. no argument.
    }
    else if(cmdIndex == 9){// fediRead Reg
        if(strcmp(token[1], "CNTR") == 0){
            cmdList[cmdIndex].args[0] = CNTR;
        }
        else if(strcmp(token[1], "DTR") == 0){
            cmdList[cmdIndex].args[0] = DTR;
        }
        else if(strcmp(token[1], "STR") == 0){
            cmdList[cmdIndex].args[0] = STR;
        }
        else if(strcmp(token[1], "MDR0") == 0){
            cmdList[cmdIndex].args[0] = MDR0;
        }
        else if(strcmp(token[1], "MDR1") == 0){
            cmdList[cmdIndex].args[0] = MDR1;
        }

        //cmdList[cmdIndex].args[0] = (int)strtol(token[1],NULL,16); // hexa number input!!// is this string???
    }
    else if(cmdIndex == 10){//fediDisp Mode
        cmdList[cmdIndex].args[0] = atoi(token[1]); // position
    }
    else if(cmdIndex == 11){//fediFw
        // do nothing. no argument.
    }
    else{
        sprintf(dispErr, "index error (-1).");
        ucsiA1UartTxString((unsigned char*)dispErr);
    }

    return cmdIndex;
}


/************************************************************************************
* Function: validateCmd
* - this function is to validate the typed command has a correct command by comparing
*   the cmdList's name which is pre-initialized.
* Arguments: (CMD*) cmdList  - array structure with pre-initialized values
*            (char*) cmdLine - input from the terminal
*
* return: index value if succeeded, -1 if failed.
* Author: Gwang Sik Kim
* Date: Jan 19, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/

int validateCmd(CMD* cmdList, char* cmdLine){
    int idx = -1;
    unsigned int i = 0;
    int invalidCmd = 1;
    while(invalidCmd && i<MAX_CMDS){
        invalidCmd = strcmp(cmdLine, cmdList[i++].name);
    }

    if (!invalidCmd){
        idx = i - 1;
    }
    return idx;
};


/************************************************************************************
* Function: executeCmd
* - this function is to execute the typed command and arguments depending on each mode
*   mode 0 : port direction
*   mode 1 : port output
*   mode 2 : p3Out output a byte
* Arguments: (CMD*) cmdList  - array structure with pre-initialized values
*            (int) index - corresponding index to indicate the command to execute
*
* return: 0 if succeeded, -1 if failed.
* Author: Gwang Sik Kim
* Date: Jan 19, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int executeCmd(CMD* cmdList, int index){
    int errIndex = -2;
    char mask;
    char byte;
    switch(index){


    // =================== PORT OUT, DIRECTION COMMAND =========================
    case 0://pDir
        mask = cmdList[index].args[1];
        if(cmdList[index].args[2] == 1){ // output set
            switch(cmdList[index].args[0]){// port number
            case 1:
                P1DIR |= mask;
                break;
            case 2:
                P2DIR |= mask;
                break;
            case 3:
                P3DIR |= mask;
                break;
            default:
                errIndex = -1;
                break;
            }
        }
        else if (cmdList[index].args[2] == 0){
            switch(cmdList[index].args[0]){ // input reset
            case 1:
                P1DIR &= ~mask;
                break;
            case 2:
                P2DIR &= ~mask;
                break;
            case 3:
                P3DIR &= ~mask;
                break;
            default:
                errIndex = -1;
                break;
            }
        }
        else{
            errIndex = -1;
        }

        break;
    case 1://pout
        mask = cmdList[index].args[1];
        if(cmdList[index].args[2] == 0){//state 0 = clears the bits
            switch(cmdList[index].args[0]){// port number
            case 1:
                P1OUT &= ~mask;
                break;
            case 2:
                P2OUT &= ~mask;
                break;
            case 3:
                P3OUT &= ~mask;
                break;
            default:
                errIndex = -1;
                break;
            }
        }
        else if (cmdList[index].args[2] == 1){// state 1 = sets the bits
            switch(cmdList[index].args[0]){// port number
            case 1:
                P1OUT |= mask;
                break;
            case 2:
                P2OUT |= mask;
                break;
            case 3:
                P3OUT |= mask;
                break;
            default:
                errIndex = -1;
                break;
            }

        }
        else if (cmdList[index].args[2] == 2){// state 2 = toggle the bits...
            switch(cmdList[index].args[0]){// port number
            case 1:
                P1OUT ^= mask;
                break;
            case 2:
                P2OUT ^= mask;
                break;
            case 3:
                P3OUT ^= mask;
                break;
            default:
                errIndex = -1;
                break;
            }
        }
        else{
            errIndex = -1;
        }
        break;
    case 2:// p3Out bytes..
        byte = cmdList[index].args[0];
        P3OUT = 0x0; // initializing
        P3OUT |= byte;
        break;
    // =================== PORT OUT, DIRECTION COMMAND =========================


    // =================== LCD COMMAND =========================
    case 3://nokLcdSetPixel
        switch(nokLcdSetPixel(cmdList[index].args[0], cmdList[index].args[1])){
        case 0://success
            errIndex = 0;
            break;
        case 1://failure
            errIndex = -1;
            break;
        default://undefined
            errIndex = -1;
            break;
        }

    case 4://nokLcdClear
        nokLcdClear();
        errIndex = 0;
        break;

    case 5://nokLcdDrawScrnLine
        switch(nokLcdDrawScrnLine(cmdList[index].args[0], cmdList[index].args[1])){
        case 0:
            errIndex = 0;
            break;
        default:
            errIndex = -1;
            break;
        }
        break;

    case 6://nokLcdDrawLine
        switch(nokLcdDrawLine(cmdList[index].args[0], cmdList[index].args[1], cmdList[index].args[2], cmdList[index].args[3])){
        case 0:
            errIndex = 0;
            break;
        default:
            errIndex = -1;
            break;
        }
        break;


        // =================== LCD COMMAND END =========================

        // =================== fedi COMMAND START =========================

    case 7://fediHome, initialization...
        posHome = (cmdList[index].args[0]);
        errIndex = fediHome();
        //errIndex = 0;
        break;
    case 8:// fediClr
        fediClr();
        errIndex = 0;
        break;
    case 9:// fediRead reg
        errIndex = fediRead(cmdList[index].args[0]);
        break;
    case 10:// fediDisp mode set
        errIndex = fediDisp(cmdList[index].args[0]);
        break;
    case 11:// fediFw
        fediFw();
        errIndex = 0;
        break;

        // =================== fedi COMMAND END =========================

    default:
        errIndex = -1;
    }

    // error index check
    if (errIndex != -1){
        errIndex = 0;
    }

    return errIndex;
}
