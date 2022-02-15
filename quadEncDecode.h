/*
 * quadEncDecode.h
 *
 *  Created on: 2021. 11. 23.
 *      Author: GwangSik
 */

#ifndef QUADENCDECODE_H_
#define QUADENCDECODE_H_


// define
#define CHAB_PORT           P1IN    // port 1
#define CHA                 BIT5    //CH A bit position
#define CHB                 BIT4    //CH B bit position

#define CHA_CURR_IDX        3
#define CHB_CURR_IDX        2
#define CHA_PREV_IDX        1
#define CHB_PREV_IDX        0

#define GEAR_RATIO          11.5
#define PULSE_PER_REV       100
#define QUADRATURE          4
#define DEG_PER_REV         360
#define MACRO_WO_ENC        1

//structure
typedef struct QUAD_ENC_DECODER{
    unsigned char channelState[4];
    // to show channel's previous/current state...
    // channelState[0] = CHB_PREV
    // channelState[1] = CHA_PREV
    // channelState[2] = CHB_CURR
    // channelState[3] = CHA_CURR

    int posCount; // position counter updated every quadrature event
} QUAD_ENC_DECODER;

//function prototypes
void qEncDecode(void);
void initqEdecoder(void);

//global variables
extern struct QUAD_ENC_DECODER qEdecoder;
extern int angleWheel;

#endif /* QUADENCDECODE_H_ */
