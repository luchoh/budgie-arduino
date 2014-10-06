/******************************************************************************\
 *
 * Name: IR.h
 *
 * Copyright: Parks Audio LLC
 *
 * Written By: Shannon Parks
 * Date Started: 9/2/2013
 *
 * Purpose:
 * Learns and writes new IR codes to EEPROM. Handles incoming IR commands.
 * Dependant on IRremote.h by Ken Shirriff:
 * https://github.com/shirriff/Arduino-IRremote
 *
 * Change Log:
 *
 \******************************************************************************/

#ifndef IR_H
#define IR_H

#include <Arduino.h>  // for type definitions
#include "mem.h"
#include "commands.h"

void IR_learn();
void IR_learn_VolumeUP();
void IR_learn_VolumeUP();
void IR_learn_VolumeDOWN();
void IR_learn_InputUP();
void IR_learn_InputDOWN();
void IR_learn_Mute();
void IR_learn_Standby();
void IR_learn_Gain();
void IR_learn_Boost();


void IR_learn()  {
    
    // learns eight IR codes and writes them to EEPROM & RAM
    // Volume UP/DOWN also have repeat codes to handle a button that stays pressed for repeated action
    IR_learn_VolumeUP();
    IR_learn_VolumeDOWN();
    IR_learn_InputUP();
    IR_learn_InputDOWN();
    IR_learn_Mute();
    IR_learn_Standby();
    IR_learn_Gain();
    IR_learn_Boost();
    
    //debug code that outputs received IR data to the com port
    Serial.print("Volume UP code:");
    Serial.println(IR_VolumeUp, HEX);
    Serial.print("Volume UP repeat code:");
    Serial.println(IR_VolumeUpRepeat, HEX);
    Serial.print("Volume DOWN code:");
    Serial.println(IR_VolumeDown, HEX);
    Serial.print("Volume DOWN repeat code:");
    Serial.println(IR_VolumeDownRepeat, HEX);
    Serial.print("Input UP code:");
    Serial.println(IR_InputUp, HEX);
    Serial.print("Input DOWN code:");
    Serial.println(IR_InputDown, HEX);
    Serial.print("Mute code:");
    Serial.println(IR_Mute, HEX);
    Serial.print("Standby code:");
    Serial.println(IR_Standby, HEX);
    Serial.print("Gain code:");
    Serial.println(IR_Gain, HEX);
    Serial.print("Bass Boost code:");
    Serial.println(IR_Boost, HEX);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rebooting Budgie");
    delay(1500);
    asm volatile ("  jmp 0");  //resets Budgie
    
} /* IR_learn */

void IR_incoming(long IRdata)  {
    //only process commands every 300mS - prevents fast changes with remotes that send repeating signals
    if ((millis() - irMillis) > 300) {
        irMillis = millis(); // resets IR delay
        if (IRdata == IR_Standby) {
            Standby();
        }
        //In STANDBY mode the Arduino is still functioning, so we disregard all IR commands other than a turn on
        if (standbyOn == 0){
            irrecv.resume();
            return;
        }
        //these two lines serially output *every* received IR data string for debugging
        Serial.print("IR code:");
        Serial.println(IRdata, HEX);
        if (IRdata == IR_Mute) {
            Mute();
            IR_Previous = IR_Mute;
        }
        if (IRdata == IR_Gain) {
            Gain();
            IR_Previous = IR_Gain;
        }
        if (IRdata == IR_Boost) {
            Boost();
            IR_Previous = IR_Boost;
        }
        if (IRdata == IR_InputUp) {
            InputUp();
            IR_Previous = IR_InputUp;
        }
        if (IRdata == IR_InputDown) {
            InputDown();
            IR_Previous = IR_InputDown;
        }
    }
    if (IRdata == IR_VolumeUp) {
        // put motor in CW motion
        VolumeUp();
        IR_Previous = IR_VolumeUp;
    }
    if (IRdata == IR_VolumeDown) {
        // put motor in CCW motion
        VolumeDown();
        IR_Previous = IR_VolumeDown;
    }
    if ((IR_Previous == IR_VolumeUp) && (IRdata == IR_VolumeUpRepeat)) {
        // put motor in CW motion
        VolumeUp();
    }
    if ((IR_Previous == IR_VolumeDown) && (IRdata == IR_VolumeDownRepeat)) {
        // put motor in CCW motion
        VolumeDown();
    }
    
    irrecv.resume(); // Receive the next value
} /* IR_incoming */

/* The following routines learn all the IR commands and overwrite the default Apple
 remote codes that are in EEPROM. */
void IR_learn_VolumeUP()  {
    //learns volume up IR command and it's associated repeat key
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press VOL+ key");
    lcd.setCursor(0, 1);
    lcd.print("for 1 second");
    while ((irrecv.decode(&results)) == 0) {
        //do nothing
    }
    IR_VolumeUp = results.value;
    EEPROM_writeAnything(50, IR_VolumeUp);
    irrecv.resume(); // Receive the next value
    while ((irrecv.decode(&results)) == 0) {
        //do nothing
    }
    IR_VolumeUpRepeat = results.value;
    EEPROM_writeAnything(54, IR_VolumeUpRepeat);
    lcd.clear();
    delay(2000);
    irrecv.resume(); // Receive the next value
} /* IR_learn_VolumeUP */

void IR_learn_VolumeDOWN()  {
    //learns volume down IR command and it's associated repeat key
    lcd.setCursor(0, 0);
    lcd.print("Press VOL- key");
    lcd.setCursor(0, 1);
    lcd.print("for 1 second");
    while ((irrecv.decode(&results)) == 0) {
        //do nothing
    }
    IR_VolumeDown = results.value;
    EEPROM_writeAnything(58, IR_VolumeDown);
    irrecv.resume(); // Receive the next value
    while ((irrecv.decode(&results)) == 0) {
        //do nothing
    }
    IR_VolumeDownRepeat = results.value;
    EEPROM_writeAnything(62, IR_VolumeDownRepeat);
    lcd.clear();
    delay(2000);
    irrecv.resume(); // Receive the next value
} /* IR_learn_VolumeDOWN */

void IR_learn_InputUP()  {
    //learns input up IR command, which advances Input 1 -> Input 2 -> Input 3 -> Input 4
    lcd.setCursor(0, 0);
    lcd.print("Press INPUT+ key");
    while ((irrecv.decode(&results)) == 0) {
        //do nothing
    }
    IR_InputUp = results.value;
    EEPROM_writeAnything(66, IR_InputUp);
    lcd.clear();
    delay(2000);
    irrecv.resume(); // Receive the next value
} /* IR_learn_InputUP */

void IR_learn_InputDOWN()  {
    //learns input down IR command, which advances Input 4 -> Input 3 -> Input 2 -> Input 1
    lcd.setCursor(0, 0);
    lcd.print("Press INPUT- key");
    while ((irrecv.decode(&results)) == 0) {
        //do nothing
    }
    IR_InputDown = results.value;
    EEPROM_writeAnything(70, IR_InputDown);
    lcd.clear();
    delay(2000);
    irrecv.resume(); // Receive the next value
} /* IR_learn_InputDOWN */

void IR_learn_Mute()  {
    //learns MUTE IR command
    lcd.setCursor(0, 0);
    lcd.print("Press MUTE key");
    while ((irrecv.decode(&results)) == 0) {
        //do nothing
    }
    IR_Mute = results.value;
    EEPROM_writeAnything(78, IR_Mute);
    lcd.clear();
    delay(2000);
    irrecv.resume(); // Receive the next value
} /* IR_learn_Mute */

void IR_learn_Standby()  {
    //learns STANDBY IR command
    lcd.setCursor(0, 0);
    lcd.print(" Press STANDBY");
    while ((irrecv.decode(&results)) == 0) {
        //do nothing
    }
    IR_Standby = results.value;
    EEPROM_writeAnything(74, IR_Standby);
    lcd.clear();
    delay(2000);
    irrecv.resume(); // Receive the next value
}  /* IR_learn_Standby */

void IR_learn_Gain()  {
    //learns Gain adjustment IR command
    lcd.setCursor(0, 0);
    lcd.print("Press GAIN key");
    while ((irrecv.decode(&results)) == 0) {
        //do nothing
    }
    IR_Gain = results.value;
    EEPROM_writeAnything(82, IR_Gain);
    lcd.clear();
    delay(2000);
    irrecv.resume(); // Receive the next value
} /* IR_learn_Gain */

void IR_learn_Boost()  {
    //learns Gain adjustment IR command
    lcd.setCursor(0, 0);
    lcd.print("Press BOOST key");
    while ((irrecv.decode(&results)) == 0) {
        //do nothing
    }
    IR_Boost = results.value;
    EEPROM_writeAnything(86, IR_Boost);
    delay(400);
    
}/* IR_learn_Boost */

#endif








