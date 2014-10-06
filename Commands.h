#ifndef COMMANDS_H
#define COMMANDS_H

#include "lcd.h"
#include "ir.h"

int UpdateAll(int x);
void SilencePopsOn();
void SilencePopsOff();
void ButtonRead();
void Standby();
void IR_learn();

void Standby() {
    //toggles Standby bit - turns off/on 24V supply to DPSDT relays and tubes
    if (standbyOn == 128){
        standbyOn = 0;  //toggles STANDBY on or off
        MuteOn = 0;
        lcd.noDisplay();
        digitalWrite(BACKLIGHT_LCD, LOW);
        analogWrite(POWER_LED, 1);
        SilencePopsOn();
        UpdateAll(0);  //turns off all the relays and puts the 595 in a high impedance state on all bits
    }
    else{
        standbyOn = 128;
        asm volatile ("  jmp 0");
    }
} /* Standby */

void Mute() {
    //toggles Mute bit and updates Status
    if (MuteOn == 0) {
        MuteOn = 1;
        Status = Status & ~64;  //AND NOT to bit 6 - toggles MUTE on
        index = 14; //resets the Mute led brightness each time
        fadeAmount = 1; //ensures fadeAmount toggle doesn't get mixed up
    }
    else {
        MuteOn = 0;
        Status = Status | 64;  //OR to bit 6 - toggles MUTE off
        digitalWrite(POWER_LED, HIGH);
    }
    UpdateAll(Status);
} /* Mute */

void MuteResume() {
    //updates Mute status when Input chnages
    if (MuteOn == 1) {
        Status = Status & ~64;  //AND NOT to bit 6 - toggles MUTE on
    }
    else {
        Status = Status | 64;  //OR to bit 6 - toggles MUTE off
    }
    UpdateAll(Status);
} /* MuteResume */

void Gain() {
    SilencePopsOn();
    //toggles Gain bit and update the current Input's register
    Status = Status ^ 16;  //Exclusive OR to 5th bit - toggles GAIN on or off
    EEPROM_writeAnything(StatusAddress, Status);
    UpdateAll(Status & ~64);
    SilencePopsOff();
} /* Gain */

void Boost() {
    SilencePopsOn();
    //set Bass Boost bit and update the current Input's register
    Status = Status ^ 32;  //Exclusive OR to 6th bit - toggles Bass Boost on or off
    EEPROM_writeAnything(StatusAddress, Status);
    UpdateAll(Status & ~64);
    SilencePopsOff();
} /* Boost */


void InputUp() {
    //goes to next input but stops at 4
    if (StatusAddress < 4) {
        SilencePopsOn();
        StatusAddress++;
        EEPROM_writeAnything(0, StatusAddress);
        
        EEPROM_readAnything(StatusAddress, Status);
        UpdateAll(Status & ~64);
        SilencePopsOff();
    }
} /* InputUp*/


void InputDown() {
    //goes to down an input but stops at 1
    if (StatusAddress > 1) {
        SilencePopsOn();
        StatusAddress--;
        EEPROM_writeAnything(0, StatusAddress);
        EEPROM_readAnything(StatusAddress, Status);
        UpdateAll(Status & ~64);
        SilencePopsOff();
    }
} /* InputDown*/

void VolumeUp() {
    digitalWrite(VOL_1A, HIGH); // set leg 1 of the H-bridge low
    digitalWrite(VOL_2A, LOW); // set leg 2 of the H-bridge high
    digitalWrite(VOL_EN, HIGH); // turns motor on
    volumeMillis = millis();
    volumeOn = 1;
} /*VolumeUp*/


void VolumeDown() {
    digitalWrite(VOL_1A, LOW); // set leg 1 of the H-bridge low
    digitalWrite(VOL_2A, HIGH); // set leg 2 of the H-bridge high
    digitalWrite(VOL_EN, HIGH); // turns motor on
    volumeMillis = millis();
    volumeOn = 1;
    
} /*VolumeDown*/


void SilencePopsOn() {
    //Enable Mute to control noise
    digitalWrite(LATCH_595, LOW);
    shiftOut(DATA_595, CLK_595, MSBFIRST, (Status & ~64));
    digitalWrite(LATCH_595, HIGH);
    delay(40);
} /*SilencePopsOn*/


void SilencePopsOff() {
    delay(40);
    //resumes previous Mute setting
    MuteResume();
} /*SilencePopsOff*/


int UpdateAll(int x) {
    //update relays
    digitalWrite(LATCH_595, LOW);
    shiftOut(DATA_595, CLK_595, MSBFIRST, x);
    digitalWrite(LATCH_595, HIGH);
    //update LCD
    UpdateLCD();
} /*UpdateAll*/

void ButtonRead() {
    buttonPressed = analogRead(BUTTONS);
    if (buttonPressed < 941) {
        oldMillis = millis();
        delay(30);    // this provides 30mS for debounce of the pushbutton switch
        buttonPressed = analogRead(BUTTONS);    // read the input pin
        do {
            //wait until button is relased
        }
        while(analogRead(BUTTONS) < 941);
        //if *any* button is pushed during a Standby state, the the unit turns on and is reset
        if (standbyOn == 0){
            standbyOn = 128;
            asm volatile ("  jmp 0");
        }
        newMillis = millis();
        //If GAIN button is held for 10+ seconds (extra second needed due to 24V sense circuit) the
        //CRC byte is erased and EEPROM returns to default values on next power up or standby cycle
        if ((newMillis - oldMillis) > 9000) {
            if ((buttonPressed > gainPressed[0]) && (buttonPressed < gainPressed[1])) {
                EEPROM.write(5, 0);
                asm volatile ("  jmp 0");  //resets Budgie
            }
            else if ((buttonPressed > boostPressed[0]) && (buttonPressed < boostPressed[1])) {
                EEPROM.write(10, !Debug);  //toggles Debug setting in EEPROM
                asm volatile ("  jmp 0");  //resets Budgie
            }
        }
        //These service buttons happen with 1+ second button presses
        else if ((newMillis - oldMillis) > 1000) {
            //Hold Mute and release for Standby
            if ((buttonPressed > mutePressed[0]) && (buttonPressed < mutePressed[1])) {
                Standby();
            }
            else if ((buttonPressed > gainPressed[0]) && (buttonPressed < gainPressed[1])) {
                //  nothing
            }
            //Hold Boost and release for remote learning mode
            else if ((buttonPressed > boostPressed[0]) && (buttonPressed < boostPressed[1])) {
                IR_learn();
            }
            //Hold Input and release to cycle through input labels
            else if ((buttonPressed > inputPressed[0]) && (buttonPressed < inputPressed[1])) {
                LabelIndex[StatusAddress - 1]++;
                if (LabelIndex[StatusAddress - 1] == 10) {
                    LabelIndex[StatusAddress - 1] = 0;
                }
                EEPROM.write((StatusAddress + 5), LabelIndex[StatusAddress - 1]);
                UpdateLCD();
            }
        }
        else {
            if ((buttonPressed > mutePressed[0]) && (buttonPressed < mutePressed[1])) {
                Mute();
            }
            else if ((buttonPressed > gainPressed[0]) && (buttonPressed < gainPressed[1])) {
                Gain();
            }
            else if ((buttonPressed > boostPressed[0]) && (buttonPressed < boostPressed[1])) {
                Boost();
            }
            else if ((buttonPressed > inputPressed[0]) && (buttonPressed < inputPressed[1])) {
                if (StatusAddress == 1) {
                    goUp = 1;
                }
                else if (StatusAddress == 4) {
                    goUp = 0;
                }
                if (goUp == true) {
                    InputUp();
                }
                else {
                    InputDown();
                }
            }
        }
    }
} /* ButtonRead */

#endif