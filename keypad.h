/*
 * keypad.h
 *
 *  Created on: 2021. 11. 9.
 *      Author: GwangSik
 */

#ifndef KEYPAD_H_  // include guards
#define KEYPAD_H_

// pre-processor statements
#define ROW         P2IN    //input port P2 <7:4> to keypad ROW<4:1>
#define COL         P6OUT   //output port P6 <3:0> to keypad COL<4:1>
#define LED         P3OUT   //output port P3 <7:0> updated with the byte, currentKeyCoord
#define ROW_DIR     P2DIR
#define COL_DIR     P6DIR
#define LED_DIR     P3DIR
#define ROW_REN     P2REN
#define ROW_PULL_DN P2OUT
#define KEY_BUFF_SZ 10
#define ROW_MASK    0xF0
#define COL1_ON     0x1
#define COL2_ON     0x2
#define COL3_ON     0x4
#define ROW1_DETECT 0x1
#define ROW2_DETECT 0x2
#define ROW3_DETECT 0x4
#define ROW4_DETECT 0x8
#define TEN_MILI_SECONDS    10000


// mode control
#define PWR         BIT0
#define RST         BIT1
#define REFRESH     BIT2
#define RST2        BIT3
#define KEY_PRESS   BIT4


// structure definitions
typedef struct KEYPAD_4X4{
    unsigned char currentKeyCoord;  // the row,col coordinate of the pressed key
    unsigned char keyPressCnt;      // number of key presses since last reset
    unsigned char keyBuffer[KEY_BUFF_SZ];       // stores history of key presses
    unsigned char currentKey;           // the literal key from Keypad
    unsigned char keyBufferPad[KEY_BUFF_SZ];
} KEYPAD_4X4;

// function prototypes
void keypadInit(void);
int getKeyPress(void);
int decodeKeyCoord(void);
void updateLedArray(void);
void keyBufferUpdate(void);

// global variable
// int* flag;
extern unsigned char debounceFlag;
extern struct KEYPAD_4X4 myKeyPad; // keypad variable

#endif /* KEYPAD_H_ */
