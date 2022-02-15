/*
 * SevenSegDisplay.h
 *
 *  Created on: 2021. 11. 23.
 *      Author: GwangSik
 */

#ifndef SEVENSEGDISPLAY_H_
#define SEVENSEGDISPLAY_H_


// define
//#define FRQ_RFRSH         33              // old refresh freq
#define FRQ_RFRSH           50
//#define INTERVAL_33HZ     30000
#define PERIOD_TOTAL        20000           //  (1 / FRQ_RFRSH) * 1000 * 1000
#define PERIOD_DIGIT        6667            // (1 / FRQ_RFRSH / 3) * 1000 * 1000
#define INTERVAL            240000
#define MACRO1              0
#define MACRO2              0
#define INTERVAL_1SEC       1000000
#define COUNT_PER_DEG       12

#define DIGIT1_IDX          0
#define DIGIT2_IDX          1
#define DIGIT3_IDX          2
#define SEG                 P3OUT           //segment port P3 <a:DP>
#define DIGIT               P4OUT           // Anode Control,P4 <2:0> is Digit 3 to Digit 1
#define SEG_DIR             P3DIR
#define DIGIT_DIR           P4DIR

#define NUM_DISP            3
#define DISP1               BIT0            // digit 1. Least Significant Digit.
#define DISP2               BIT1            // digit 2.
#define DISP3               BIT2            // digit 3. Most significant Digit.
#define SEG_a               BIT0
#define SEG_b               BIT1
#define SEG_c               BIT2
#define SEG_d               BIT3
#define SEG_e               BIT4
#define SEG_f               BIT5
#define SEG_g               BIT6
#define SEG_DP              BIT7            //negative sign
// wiring has flipped so all SEG bits are flipped from a to DP.


//structure
typedef struct SEVEN_SEG_DISP{
    char decDigit;                  //base 10 decimal digit, -9 to +9
    unsigned char binSegCode;       //7 segment code <a:g> plus dp
    unsigned char ctrl;             //single bit indicating On/OFF state

} SEVEN_SEG_DISP;

//function prototypes
void sevenSegDispInit(struct SEVEN_SEG_DISP* display);
void decTo7Seg(struct SEVEN_SEG_DISP* display);
void dispRefresh(struct SEVEN_SEG_DISP* display, unsigned char dispIndex);

//global variables
extern unsigned char sysState; // sysState.0 : PWR , sysState.1 : CLR, sysState.2 : REFRESH, sysState.3 : FLASH, sysState.4 : KEY_PRESS.


#endif /* SEVENSEGDISPLAY_H_ */
