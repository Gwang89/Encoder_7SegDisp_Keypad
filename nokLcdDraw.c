/*
 * nokLcdDraw.c
 *
 *  Created on: 2022. 2. 8.
 *      Author: GwangSik
 */


#include <msp430.h>
#include "nok5110LCD.h"
#include "nokLcdDraw.h"
#include <math.h>


int idxPrev = 0;

int xPosPrev;
int revPrev;

double xLocation;


static long signed int posCountTotal = 0;

// posCount -> LCD display updated with flywheel angle.




// pseudo-code
/*
 * 1. Line Mode
 * when the function is called, input argument = posCount
 * this function is called by TimerA0, so 250ms period..
 *
 * let's say 15 degree is a minimum degree to represent.
 * so, 15 deg, 30deg, 45 deg, 60 deg, 75 deg, 90 deg, and so on.
 * it seems posCount = 4600 counts at max.
 * 4600 / 360 * 15 = 191.67 posCount.
 * thus, when the posCount is greater than POSCOUNT_MIN_DEG * index
 * it gives index number, which ranges from 0 to 24.
 * (0 means posCount from 0 to 191.67)
 * (1 means posCount from 191.67 to 383.3, which is 191.67 * 2)
 * according to this index number,
 * we can set pre-defined x,y coordinates to draw the single line
 * to represent its angle...
 *
 *
 *
 *
 * **** assume posCount - posHome is already calculated before this function is called...
 *
 *  (1)
 *  if (posCount >= 0){
 *      posCount % 4600 // confined its range up to 4600. one revolution
 *  }
 *  else{
 *      posCount += 4600 // force posCount to be in a range of 0 to 4600.
 *
 *  (2) find_index_number
 *      if (posCount is between 0 to 191 ( = 4600 / 360 * 15 )), index = 0
 *      else if (posCount is between 191 to 191 * 2), index = 1.
 *      keep going...
 *      else if (posCount is between 191 * 23 to 191 * 24), index = 23.
 *      else if (posCount is between 191 * 24 to 191 * 25), index = 24. -- last range.
 *      else{
 *          index = -1. // error case.
 *      }
 *
 *  (3) update LCD screen.
 *   if posCount has been changed
 *      LCD_clear
 *      draw_line_from_center_point_to_corresponding_point_matched_to_index
 *      // in this case, we use pre-defined point as a destination point
 *      // following the corresponding index value.
 *      // for example, if it's 90 deg,
 *      // nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 60, 20)
 *      // need to define it in .h file.
 *
 *   (4) update indexPrev = index (current)
 *              posCountPrev= posCount
 *              // this has to be done so that when it doesn't have to be drawn, don't update it...
 *   (5) return errorIndex.
 *
 *
 *
 *
 *
 */


/************************************************************************************
* Function: nokLcdDispLineMode
* - to display single line to indicate what angle it is now based on posCount
* argument:
*       (void)
*       global variable posCntFrHm is being used - current posCount read from LS7366R
* return: (int) 0 to 24 success (index indicates the angle * minimum_angle / -1 failure
* Author: Gwang Sik Kim
* Date: Feb 14, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
//int nokLcdDispLineMode(long int posCnt){
int nokLcdDispLineMode(void){
    posCountTotal = posCntFrHm;

    if(posCountTotal >= 0){
        posCountTotal = posCntFrHm % POSCOUNT_PER_REV;
    }
    else{
        while(posCountTotal < 0){
            posCountTotal += POSCOUNT_PER_REV;
        }
    }
    int index = -1;

    if(posCountTotal >=0 && posCountTotal < (POSCOUNT_MIN_DEG * 1)){
        index = 0;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 2) {
        index = 1;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 3){
        index = 2;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 4){
        index = 3;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 5){
        index = 4;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 6){
        index = 5;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 7){
        index = 6;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 8){
        index = 7;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 9){
        index = 8;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 10){
        index = 9;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 11){
        index = 10;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 12){
        index = 11;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 13){
        index = 12;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 14){
        index = 13;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 15){
        index = 14;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 16){
        index = 15;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 17){
        index = 16;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 18){
        index = 17;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 19){
        index = 18;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 20){
        index = 19;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 21){
        index = 20;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 22){
        index = 21;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 23){
        index = 22;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 24){
        index = 23;
    }
    else if(posCountTotal < POSCOUNT_MIN_DEG * 25){
        index = 24;
    }
    else{
        index = -1;
    }

    // index comparison and update it only when it's different

    if(index == idxPrev){
        // do nothing
    }
    else{//update
        nokLcdClear();
        switch(index){
            case 0:
                LINE_MODE_DEG_0;
                break;
            case 1:
                LINE_MODE_DEG_15;
                break;
            case 2:
                LINE_MODE_DEG_30;
                break;
            case 3:
                LINE_MODE_DEG_45;
                break;
            case 4:
                LINE_MODE_DEG_60;
                break;
            case 5:
                LINE_MODE_DEG_75;
                break;
            case 6:
                LINE_MODE_DEG_90;
                break;
            case 7:
                LINE_MODE_DEG_105;
                break;
            case 8:
                LINE_MODE_DEG_120;
                break;
            case 9:
                LINE_MODE_DEG_135;
                break;
            case 10:
                LINE_MODE_DEG_150;
                break;
            case 11:
                LINE_MODE_DEG_165;
                break;
            case 12:
                LINE_MODE_DEG_180;
                break;
            case 13:
                LINE_MODE_DEG_195;
                break;
            case 14:
                LINE_MODE_DEG_210;
                break;
            case 15:
                LINE_MODE_DEG_225;
                break;
            case 16:
                LINE_MODE_DEG_240;
                break;
            case 17:
                LINE_MODE_DEG_255;
                break;
            case 18:
                LINE_MODE_DEG_270;
                break;
            case 19:
                LINE_MODE_DEG_285;
                break;
            case 20:
                LINE_MODE_DEG_300;
                break;
            case 21:
                LINE_MODE_DEG_315;
                break;
            case 22:
                LINE_MODE_DEG_330;
                break;
            case 23:
                LINE_MODE_DEG_345;
                break;
            case 24:
                LINE_MODE_DEG_0;
                break;
            default:
                //index = -1;
                break;
            }
    }
    idxPrev = index;
    posCountPrev = posCntFrHm;
    return index;
}







/*
 *
 *
 * pseudo-code
 * 2. Horizontal Bar Mode
 * (1) check if posCount is updated.
 * (2) if posCount >= 0
 *          positive mode. represent current posCount as a filled pixel.
 *          set the number of pixels of corresponding the posCount value.
 *          from the 0,0 origin.
 *
 *          if posCount is increasing --> mode 1
 *              it advances from left to right towards (83,47).
 *          else (posCount is decreasing) --> mode 2
 *              it retreats from right to left towards 0,0 origin.

 *     else (posCount< 0)
 *          negative mode. represent current posCount as a filled pixel but negative way.
 *          set the number of pixels of corresponding the posCount value.
 *          from the last pixel, (83, 47). it goes backwards, from right to left.
 *
 * *          if posCount is increasing --> mode 3
 *              it advances from left to right towards but it gradually disappears.. (because it's increasing)
 *          else (posCount is decreasing) --> mode 4 (more negative)
 *              it retreats from right to left towards 0,0 origin but it gradually sets the pixels...
 *
 * (3) to achieve this, buffer1 = posCount -> make it within the range of 4600 * 6
 *  --> we will use this instead of posCount, and this value is considered as a target pixels to represent!
 *
 * (4) calculate revolution (bank number) and xPosition using that posCountBuffer value.
 *   --> so that we now know where our target pixel is placed.
 *
 *  (5) depending on the mode (positive vs negative , advance vs retreat)
 *   decide whether the pixels are getting filled or erased.
 *   referred to the previous count value.
 *   --> which means we need to store the xPosPrevious and revolutionPrevious values.
 *   --> compare two values and we can decide whether we are going backwards/forwards.
 *
 *
 * =========================================================================================
 * (1) // mode = 0 to 4
 *     // mode 0 : no update.
 *     // mode 1 : positive mode, advance.
 *     // mode 2 : negative mode, retreat
 *     // mode 3 : negative mode. increasing posCount (towards positive)
 *     // mode 4 : negative mode, decreasing pos Count (towards more negative)
 *
 *    if (posCntPrev == posCnt){
 *      mode = 0. no updates.
 *      break.
 *    }
 *
 * (2) mode setting.
 *    if (posCount > 0)
 *       if (posCount > posCountPrev), mode = 1
 *       else, mode = 2
 *
 *    else
 *       if (posCount > posCountPrev), mode = 3
 *       else, mode = 4
 *
 * (3) calculate how many lines (rev) and columns (xPos) are needed...
 *   posCountTotal = posCount; //copy
 *   posCountTotal = posCountTotal mod ( 4600 * 6 )//posCountTotal is within 4600 * 6 range...
 *
 * (4)
 *   rev = posCountTotal mod 4600 // how many turns we are...
 *   xPos = posCountTotal / 4600 * 84 // what column we should advance...
 *
 *
 * (5) set pixels...
 *  switch(mode){
 *  // each mode is also divided into two cases; one is when the updated posCount and the previous posCount are in the same line
 *  // and the other is when they are in different lines.
 *  mode 1:
 *      if (the line is the same)
 *      for loop
 *          draw the line from previous xPos to current xPos (fill the pixels. 0xFF)
 *
 *      if (the line is changed)
 *          for loop
 *              draw the entire line in the previous banks   (here, bank is lower number!)
 *          for loop
 *              draw the line from 0 to the current xPos
 *  mode 2:
 *      if (the line is the same)
 *      for loop
 *          clear the line from previous xPos to current xPos (clear the pixels. 0x00)
 *
 *      if (the line is changed)
 *          for loop
 *              clear the entire line in the previous banks
 *          for loop
 *              clear the line from 83 (max_col_xPos) to the current xPos
 *
 *
 *  mode 3://negative mode, increasing posCount... clear the pixels..
 *      if (the line is the same)
 *      for loop
 *          draw the line from previous xPos to current xPos
 *
 *      if (the line is changed)
 *          for loop
 *              draw the entire line in the previous banks
 *          for loop
 *              draw the line from 0 to the current xPos
 *
 *
 *  mode 4://negative mode, decreasing posCount... fill the pixels.
 *      if (the line is the same)
 *      for loop
 *          draw the line from previous xPos to current xPos (fill the pixels, 0xFF)
 *
 *      if (the line is changed)
 *          for loop
 *              draw the entire line in the previous banks (here, bank is higher number!)
 *          for loop
 *              draw the line from MAX_COL_xPos to the current xPos
 *
 *
 *
 *  }
 *
 *
 * (6) //update previous values to the current values
 *     posCountPrev = posCount
 *     xPosPrev = xPos
 *     revPrev = rev
 *
 * (7)  return errIdx // -1 failed. 0 to 4 -> mode index.
 *
 */


/************************************************************************************
* Function: nokLcdDispBarMode
* - to display progress bar to indicate what angle and how many turns it is now based on posCount
* argument:
*       (void)
*       global variable posCntFrHm is being used
* return: (int) 0 to 4 (mode indicates how the progress proceeds / -1 failure
* Author: Gwang Sik Kim
* Date: Feb 14, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
int nokLcdDispBarMode(void){
    int errIdx = -1;
    //nokLcdClear(); // update, refresh.
    char mode;

    if (posCntFrHm == posCountPrev){
    //if (posCnt == posCountPrev){
        mode = 0;
        return errIdx = 0; // no update. stationary.
    }

    int rev; // can count up to 6 revs



    // ======================================================================//
    if(posCntFrHm >= 0){ // positive mode
        //posCountTotal = posCnt;
        posCountTotal = posCntFrHm;



        //if(posCnt > POSCOUNT_MAX_LCD){
        //    nokLcdClear();
        //}
        while(posCountTotal > (POSCOUNT_MAX_LCD)){
            posCountTotal -= POSCOUNT_MAX_LCD;
        }// posCountTotal counts up to the maximum number of LCD posCount, 4600 * 6

    }
    else{ // negative mode
        posCountTotal = posCntFrHm;
        //posCountTotal = posCnt;
        while(posCountTotal < (0)){
            posCountTotal += POSCOUNT_MAX_LCD;
        }// posCountTotal counts up to the maximum number of LCD posCount, 4600 * 6.
    }


    //rev calculation based on posCountTotal
    if (posCountTotal <= POSCOUNT_PER_REV ) rev = 0;
    else if (posCountTotal <= POSCOUNT_PER_REV * 2) rev = 1;
    else if (posCountTotal <= POSCOUNT_PER_REV * 3) rev = 2;
    else if (posCountTotal <= POSCOUNT_PER_REV * 4) rev = 3;
    else if (posCountTotal <= POSCOUNT_PER_REV * 5) rev = 4;
    else if (posCountTotal <= POSCOUNT_PER_REV * 6) rev = 5;
    // posCountTotal is re-calculated based on 4600, per revolution.
    while(posCountTotal > (POSCOUNT_PER_REV)){
        posCountTotal -= POSCOUNT_PER_REV;
    }
    // calculation for the last x-position, the rest of the lines are fully filled up
    xLocation = posCountTotal * LCD_MAX_COL / POSCOUNT_PER_REV;


    if (posCntFrHm >= 0){
        if (posCntFrHm > posCountPrev){
            mode = 1; // advance
        }
        else if (posCntFrHm < posCountPrev){
            mode = 2; // retreat
        }
    }
    else{
        if (posCntFrHm > posCountPrev){
            mode = 3; // advance
        }
        else if (posCntFrHm < posCountPrev){
            mode = 4; // retreat
        }
    }
    /*
    if (*posCount >= 0){
        if (posCnt > posCountPrev){
            mode = 1; // advance
        }
        else if (posCnt < posCountPrev){
            mode = 2; // retreat
        }
    }
    else{
        if (posCnt > posCountPrev){
            mode = 3; // advance
        }
        else if (posCnt < posCountPrev){
            mode = 4; // retreat
        }
    }
    */


    if (revPrev > rev && mode == 1){
        nokLcdClear();
    }

    if(revPrev < rev && mode == 2){
        nokLcdClear();
        nokLcdFull();
    }

    if(revPrev > rev && mode == 3){
        nokLcdClear();
    }
    if(revPrev < rev && mode == 4){
        nokLcdClear();
    }




    int bank;
    int xPos;
    //int xPosProgress = (int) xLocation; // this is for one revolution

    switch(mode){
    case 1:// positive, advance.
        if(revPrev < rev){//different revolutions. fill the previous revolutions first
            for (bank=revPrev; bank<rev; bank++){
                for (xPos=0; xPos<LCD_MAX_COL; xPos++){
                    // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                    nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                    nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);
                    nokLcdWrite(0xFF, DC_DAT); // write the data. this is DATA DC_DAT
                }
            }
            for (xPos=0; xPos< (int) xLocation; xPos++){// and newly fill the pixels up to the new location..
                // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0xFF, DC_DAT); // write the data. this is DATA DC_DAT
            }
        }
        else{// same revolutions. fill the pixels from previous to current location.
            for (xPos=xPosPrev; xPos< (int) xLocation; xPos++){
                // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0xFF, DC_DAT); // write the data. this is DATA DC_DAT
            }
            for(bank = rev + 1; bank < LCD_MAX_BANK; bank++){
                for (xPos=0; xPos<LCD_MAX_COL ; xPos++){
                    // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                    nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                    nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);
                    nokLcdWrite(0x0, DC_DAT); // write the data. this is DATA DC_DAT
                }
            }


        }
        break;

    case 2:// positive, retreat
        if(revPrev > rev){
            for (bank=0; bank<rev; bank++){
                for (xPos=0; xPos<LCD_MAX_COL; xPos++){
                    // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                    nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                    nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);
                    nokLcdWrite(0xFF, DC_DAT); // write the data. this is DATA DC_DAT
                }
            }
            for (xPos=0; xPos<(int) xLocation ; xPos++){
                // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0xFF, DC_DAT); // write the data. this is DATA DC_DAT
            }
            for (xPos=(int) xLocation; xPos<LCD_MAX_COL ; xPos++){
                // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0x0, DC_DAT); // write the data. this is DATA DC_DAT
            }
            for (xPos=0; xPos<LCD_MAX_COL ; xPos++){
                // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | revPrev, DC_CMD);
                nokLcdWrite(0x0, DC_DAT); // write the data. this is DATA DC_DAT
            }
        }
        else{
            for (xPos=(int) xLocation; xPos<LCD_MAX_COL ; xPos++){
                // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0x0, DC_DAT); // write the data. this is DATA DC_DAT
            }
        }
        break;

    case 3:// negative, increasing
        // for the very first time, it has a flaw.... let's see how it works with the encoder module.
        if(revPrev != rev){
            for (bank=0; bank<=revPrev; bank++){
                for (xPos=0; xPos<LCD_MAX_COL; xPos++){
                    nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                    nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);
                    nokLcdWrite(0x0, DC_DAT);
                }
            }
            for (xPos=0; xPos< (int) xLocation; xPos++){
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0x0, DC_DAT);
            }
            for (xPos=(int) xLocation; xPos< LCD_MAX_COL; xPos++){
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0xFF, DC_DAT);
            }
            for (bank=rev + 1; bank<LCD_MAX_BANK; bank++){
                for (xPos=0; xPos<LCD_MAX_COL; xPos++){
                    nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                    nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);
                    nokLcdWrite(0xFF, DC_DAT);
                }
            }

        }
        else{
            for (xPos=0; xPos<(int) xLocation ; xPos++){
                // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0x0, DC_DAT); // write the data. this is DATA DC_DAT
            }
            for (xPos=(int) xLocation; xPos<LCD_MAX_COL ; xPos++){
                // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0xFF, DC_DAT); // write the data. this is DATA DC_DAT
            }
        }
        break;

    case 4:// negative, decreasing (more negative)
        if(revPrev != rev){
            for (bank=0; bank<=revPrev; bank++){
                for (xPos=0; xPos<LCD_MAX_COL; xPos++){
                    nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                    nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);
                    nokLcdWrite(0x0, DC_DAT);
                }
            }
            for (xPos=0; xPos< (int) xLocation; xPos++){
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0x0, DC_DAT);
            }
            for (xPos=(int) xLocation; xPos< LCD_MAX_COL; xPos++){
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0xFF, DC_DAT);
            }
            for (bank=rev + 1; bank<LCD_MAX_BANK; bank++){
                for (xPos=0; xPos<LCD_MAX_COL; xPos++){
                    nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                    nokLcdWrite(LCD_SET_YRAM | bank, DC_CMD);
                    nokLcdWrite(0xFF, DC_DAT);
                }
            }
        }
        else{
            for (xPos=0; xPos<(int) xLocation ; xPos++){
                // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0x0, DC_DAT); // write the data. this is DATA DC_DAT
            }
            for (xPos=(int) xLocation; xPos<LCD_MAX_COL ; xPos++){
                // set the x and y RAM address  corresponding to the desired (x,bank) location. this is a command DC_CMD
                nokLcdWrite(LCD_SET_XRAM | xPos, DC_CMD);
                nokLcdWrite(LCD_SET_YRAM | rev, DC_CMD);
                nokLcdWrite(0xFF, DC_DAT); // write the data. this is DATA DC_DAT
            }
        }
        break;
    default:
        errIdx = -1;
        break;
    }
    // update previous posCount
    //posCountPrev = posCnt;
    posCountPrev = posCntFrHm;
    if(xPos == 84) {
        xPosPrev = 0;
    }
    else xPosPrev = xPos;


    revPrev = rev;
    if( errIdx == -1){
        return errIdx;
    }
    else{
        return mode;
    }

}


/************************************************************************************
* Function: initPrev
* - to initialize previous value in the beginning, based on the posCount defined.
* argument: (void) n/a
* return: (void) n/a
* Author: Gwang Sik Kim
* Date: Feb 14, 2022
* Modified: <date of any mods> usually taken care of by rev control
************************************************************************************/
void initPrev(void){
    long int posCountTotal;
    int rev;
    // ======================================================================//
    if(*posCount >= 0){ // positive mode
        posCountTotal = *posCount;
        while(posCountTotal > (POSCOUNT_MAX_LCD)){
            posCountTotal -= POSCOUNT_MAX_LCD;
        }// posCountTotal counts up to the maximum number of LCD posCount, 4600 * 6

    }
    else{ // negative mode
        posCountTotal = *posCount;
        while(posCountTotal < (0)){
            posCountTotal += POSCOUNT_MAX_LCD;
        }// posCountTotal counts up to the maximum number of LCD posCount, 4600 * 6.
    }


    //rev calculation based on posCountTotal
    if (posCountTotal <= POSCOUNT_PER_REV ) rev = 0;
    else if (posCountTotal <= POSCOUNT_PER_REV * 2) rev = 1;
    else if (posCountTotal <= POSCOUNT_PER_REV * 3) rev = 2;
    else if (posCountTotal <= POSCOUNT_PER_REV * 4) rev = 3;
    else if (posCountTotal <= POSCOUNT_PER_REV * 5) rev = 4;
    else if (posCountTotal <= POSCOUNT_PER_REV * 6) rev = 5;
    // posCountTotal is re-calculated based on 4600, per revolution.
    while(posCountTotal > (POSCOUNT_PER_REV)){
        posCountTotal -= POSCOUNT_PER_REV;
    }
    // calculation for the last x-position, the rest of the lines are fully filled up
    xLocation = posCountTotal * LCD_MAX_COL / POSCOUNT_PER_REV;


    xPosPrev = (int) xLocation;
    revPrev = rev;
    posCountPrev = *posCount - posHome;

}
