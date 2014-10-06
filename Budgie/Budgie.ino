/******************************************************************************\
 *
 * Name: budgiesketchV0_05.ino
 *
 * Copyright: Parks Audio LLC
 *
 * Written By: Shannon Parks
 * Date Started: 9/2/2013
 *
 * Purpose:
 * Sketch for the Parks Audio Budgie Preamplifier.
 *
 * Change Log:
 *            ver 0.04 - Initial release.
 *            ver 0.05 - Added Debug Mode - turns off 24V sense and 15 second heater wait.
 *                       Cleaned up button code at bottom of commands.h.
 *                       Serially outputs all received IR data for debugging
 *                       Fixed Volume IR repeat code bug
 *
 \******************************************************************************/

//external libraries
#include <LiquidCrystal.h>
#include <IRremote.h>
#include <EEPROM.h>

//local libraries
#include "EEPROMAnything.h"
#include "mem.h"
#include "ir.h"
#include "lcd.h"
#include "commands.h"
#include "setup.h"

//variables are declared in mem.h

//void setup() has been spun off as a separate setup.h file
void setup();

void loop() {
    
    //process any incoming IR code
    if (irrecv.decode(&results)) IR_incoming(results.value);
    
    //scan the four button array
    ButtonRead();
    
    //if the unit is Muted, pulse the power led slowly.
    if ((MuteOn == 1) && ((millis() - muteMillis) > 100)) {
        muteMillis = millis();
        analogWrite(POWER_LED, brightness[index]);
        if (index == 0 || index == 14) {
            fadeAmount = -fadeAmount ;
        }
        index = index + fadeAmount;
    } /* if Mute LED */
    
    //This turns off a volume UP or DOWN button press after 500ms
    if ((volumeOn == 1) && ((millis() - volumeMillis) > 500)) {
        volumeOn = 0;
        digitalWrite(VOL_EN, LOW); // turns motor off
    } /* if Volume On   */
    
    //monitors the 24V at the filaments for a muted shutdown event
    monitored24Vsupply = analogRead(BROWNOUT);
    //if our 24V drops 2% and we had a valid voltage in the first place, then mute the amp for shutdown
    if ((monitored24Vsupply <= vinCheck) && (Debug == 0)){
        //enable Mute to control shutdown thump
        digitalWrite(LATCH_595, LOW);
        shiftOut(DATA_595, CLK_595, MSBFIRST, (Status & ~64));
        digitalWrite(LATCH_595, HIGH);
        //turns off our LCD backlight
        digitalWrite(BACKLIGHT_LCD, LOW);
        //turns off our power led
        digitalWrite(POWER_LED, LOW);
        //unit shutdowns here during delay
        delay(1000);
    }
    //updates the montitored 24V every 2 seconds to take care of normal voltage variations
    if ((millis() - brownoutMillis) > 2000) {
        brownoutMillis = millis();
        vinCheck = monitored24Vsupply * .99;
    }
    if (monitored24Vsupply < 512) {
        //insert code that deals with missing tubes or 24V supply issues
        //unfortunately, a lot of brute force code would screw up debugging
        //the unit with only USB power - needs good implementation
    }
    
} /*main loop*/