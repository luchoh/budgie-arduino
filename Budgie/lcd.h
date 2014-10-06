/******************************************************************************\
 *
 * Name: lcd.h
 *
 * Copyright: Parks Audio LLC
 *
 * Written By: Shannon Parks
 * Date Started: 9/2/2013
 *
 * Purpose:
 * Has setup for LCD and controls formatting and refreshes for the 1602 LCD.
 *
 * Change Log:
 *
 \******************************************************************************/

#ifndef LCD_H
#define LCD_H

#include "commands.h"

void UpdateFirstLine();
void UpdateSecondLine();

void UpdateLCD() {
    UpdateFirstLine();
    UpdateSecondLine();
} /* UpdateLCD */

void UpdateFirstLine() {
    //Displays Input Channel Information
    switch (Status & 15) {
        case 8:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("[1] ");
            lcd.print(inputLabel[LabelIndex[0]]);
            break;
        case 4:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("[2] ");
            lcd.print(inputLabel[LabelIndex[1]]);
            break;
        case 2:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("[3] ");
            lcd.print(inputLabel[LabelIndex[2]]);
            break;
        case 1:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("[4] ");
            lcd.print(inputLabel[LabelIndex[3]]);
            break;
        default:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("[1] ");
            lcd.print(inputLabel[LabelIndex[0]]);
    }
} /* UpdateFirstLine */

void UpdateSecondLine() {
    if (MuteOn == 1) {
        lcd.setCursor(0,1);
        lcd.print("      MUTE      ");
    }
    else
    {
        //Displays Gain Setting Information
        if (Status & (1 << 4)) {
            lcd.setCursor(8,1);
            lcd.print("Gain 6dB");
        }
        else {
            lcd.setCursor(8,1);
            lcd.print("Gain 0dB");
        }
        
        //Displays Bass Boost Setting
        if (Status & (1 << 5)) {
            lcd.setCursor(0,1);
            lcd.print("Bass EQ");
        }
        else {
            lcd.setCursor(0,1);
            lcd.print("Flat EQ");
        }
    }
} /* UpdateSecondLine */

void Setup_LCD()  {
    lcd.display();
    lcd.begin(16, 2);
    // LCD splashscreen
    lcd.print(" Budgie Preamp");
    lcd.setCursor(0,1);
    lcd.print("     v0.05");
    delay(1000);
    lcd.clear();
    lcd.print("Arduino Powered");
    lcd.setCursor(0,1);
    lcd.print(" by Parks Audio");
    delay(1000);
    lcd.clear();
    //delay for muting circuit as tubes are warming up
    //this prevents weird crackily noises and lack of amplification
    if (Debug == 0) {
        for(int z = 15; z > 0; z--){
            lcd.setCursor(0,0);
            lcd.print("Tubes Warming Up");
            lcd.setCursor(0,1);
            lcd.print(z, DEC);
            lcd.print(" seconds left ");
            delay(1000);
        } 
    }
} /* Setup_LCD */

#endif