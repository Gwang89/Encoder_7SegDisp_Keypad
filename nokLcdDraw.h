/*
 * nokLcdDraw.h
 *
 *  Created on: 2022. 2. 8.
 *      Author: GwangSik
 */

#ifndef NOKLCDDRAW_H_
#define NOKLCDDRAW_H_


#define     MINIMUM_DEG         15
#define     POSCOUNT_PER_REV    4600
#define     DEG_PER_REV         360
#define     POSCOUNT_MAX_LCD    4600 * 6
#define     POSCOUNT_MIN_DEG    POSCOUNT_PER_REV / DEG_PER_REV * MINIMUM_DEG
#define     PI_CONST            3.1415926535
#define     LINE_MODE_CTR_X         41
#define     LINE_MODE_CTR_Y         20
#define     LINE_MODE_DEG_0         nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 41, 1)
#define     LINE_MODE_DEG_15        nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 46, 2)
#define     LINE_MODE_DEG_30        nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 50, 4)
#define     LINE_MODE_DEG_45        nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 54, 7)
#define     LINE_MODE_DEG_60        nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 57, 11)
#define     LINE_MODE_DEG_75        nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 59, 15)
#define     LINE_MODE_DEG_90        nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 60, 20)
#define     LINE_MODE_DEG_105       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 59, 26)
#define     LINE_MODE_DEG_120       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 57, 29)
#define     LINE_MODE_DEG_135       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 54, 33)
#define     LINE_MODE_DEG_150       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 50, 36)
#define     LINE_MODE_DEG_165       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 46, 38)
#define     LINE_MODE_DEG_180       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 41, 39)
#define     LINE_MODE_DEG_195       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 36, 38)
#define     LINE_MODE_DEG_210       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 32, 36)
#define     LINE_MODE_DEG_225       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 28, 33)
#define     LINE_MODE_DEG_240       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 25, 29)
#define     LINE_MODE_DEG_255       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 23, 26)
#define     LINE_MODE_DEG_270       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 22, 20)
#define     LINE_MODE_DEG_285       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 23, 15)
#define     LINE_MODE_DEG_300       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 25, 11)
#define     LINE_MODE_DEG_315       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 28, 7)
#define     LINE_MODE_DEG_330       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 32, 4)
#define     LINE_MODE_DEG_345       nokLcdDrawLine(LINE_MODE_CTR_X, LINE_MODE_CTR_Y, 36, 2)



int nokLcdDispLineMode(void);
int nokLcdDispBarMode(void);
double degToRad(double degAng);
double radToDeg(double radAng);
void nokLcdWriteXY(int xLoc1, int xLoc2, int yBank1, int yBank2, int wrt);
void initPrev(void);

//extern unsigned char currentPixelDisplay[LCD_MAX_COL][LCD_MAX_ROW / LCD_ROW_IN_BANK];
extern long signed int posCountPrev;
extern long signed int* posCount;
extern long signed int posHome;
extern long int posCntFrHm;

#endif /* NOKLCDDRAW_H_ */
