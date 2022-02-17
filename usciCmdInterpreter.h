/*************************************************************************************************
 * ucsiUart.h
 * - - C interface file for MSP430 UCSI UART A1, A0
 *
 *  Author: Greg Scutt
 *  Created on: March 1, 2017
 *  Modified: Feb 19, 2018
 **************************************************************************************************/

#ifndef USCICMDINTERPRETER_H_
#define USCICMDINTERPRETER_H_

#define     TXD_A1          BIT4            //Transmit Data on P4.4
#define     RXD_A1          BIT5            //Recieve Data on P4.5
#define     BUFF_SIZE       100
#define     ASCII_CR        0x0D
#define     ASCII_LF        0x0A
#define     ASCII_NULL      0x00
#define     MAX_ARGS        4
#define     MAX_CMDS        12

// Lab 1 - 2
#define     CMD0_NAME       "pDir"
#define     CMD0_NARGS      3
#define     CMD1_NAME       "pOut"
#define     CMD1_NARGS      3
#define     CMD2_NAME       "p3Out"
#define     CMD2_NARGS      1

// Lab 3 - LCD
#define     CMD3_NAME       "nokLcdSetPixel"
#define     CMD3_NARGS      2 // x and y coordinates
#define     CMD4_NAME       "nokLcdClear"
#define     CMD4_NARGS      0 //
#define     CMD5_NAME       "nokLcdDrawScrnLine"
#define     CMD5_NARGS      2 // linePos and hor/ver
#define     CMD6_NAME       "nokLcdDrawLine"
#define     CMD6_NARGS      4 // x1,y1, x2, y2


// Lab 4 - LS7366 Encoder ...
#define     CMD7_NAME       "fediHome"
#define     CMD7_NARGS      1
#define     CMD8_NAME       "fediClr"
#define     CMD8_NARGS      0
#define     CMD9_NAME       "fediRead"
#define     CMD9_NARGS      1
#define     CMD10_NAME       "fediDisp"
#define     CMD10_NARGS     1
#define     CMD11_NAME       "fediFw"
#define     CMD11_NARGS     0

// structure
// should be above prototype, so that linker understands what structure is being used. ..
typedef struct CMD{
    const char* name;
    int nArgs;
    int args[MAX_ARGS];
}CMD;


// prototype
void initCmdList(CMD* cmds);
int parseCmd(CMD* cmdList, char* cmdLine);
int validateCmd(CMD* cmdList, char* cmdLine);
//void hexCharToInt(char* ch, int* num);
int executeCmd(CMD* cmdList, int index);


extern unsigned char* bufferReg;

#endif /* USCICMDINTERPRETER_H_ */
