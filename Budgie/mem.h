/******************************************************************************\
 *
 * Name: mem.h
 *
 * Copyright: Parks Audio LLC
 *
 * Written By: Shannon Parks
 * Date Started: 9/2/2013
 *
 * Purpose:
 * Global memory file.  Contains all global variables used in the project.
 *
 * Change Log:
 *
 \******************************************************************************/

#ifndef MEM_H
#define MEM_H

#include <Arduino.h>  // for type definitions

// Digital pin assignment notes
// Analog pins used as digital are considered D14 thru D18 (e.g. A0 is D14)
//
// D0 and D1 are reserved for serial bus (TX & RX)
// D13 can only used as an output as there's a status LED is on that port
// SPI bus is not used other than for digital pins (no MOSI, MISO or SCK)
// D2 is the only spare pin, but it supports hardware interrupts and PWM
const byte CLK_595 = 3;
const byte LATCH_595 = 4;
const byte ENABLE_595 = 5;
const byte POWER_LED = 6;
const byte DATA_595 = 7;
const byte IR = 8;
const byte BACKLIGHT_LCD = 9;
const byte VOL_2A = 10;
const byte VOL_1A = 11;
const byte VOL_EN = 12;
const byte D7_LCD = 13;
const byte D6_LCD = 14;
const byte D5_LCD = 15;
const byte D4_LCD = 16;
const byte EN_LCD = 17;
const byte RS_LCD = 18;
const byte BUTTONS = 19;
const byte BROWNOUT = 20;

/*
 Status is the control register for all the relays (0 off, 1 on)
 Mute & Standby settings do not get stored in EEPROM and are runtime decisions
 E.g. B00010010 (18 decimal) would be Input 2 with Bass Boost enabled.
 bit 0 - Input 4
 bit 1 - Input 3
 bit 2 - Input 2
 bit 3 - Input 1
 bit 4 - Bass Boost
 bit 5 - Gain
 bit 6 - Mute
 bit 7 - Standby
 
 Default state is with all the DPDT relays OFF. Inputs 1-4 will be opens.
 MUTE will short output to GND. SPST Standby relay is Normally Open and turned off.
 Initial value for Status turns on Standby relay so 24V gets turned on to tubes and DPDT relays
 */
byte Status = B10000000;
byte StatusAddress = 1;
byte Input1 = B11001000;
byte Input2 = B11000100;
byte Input3 = B11000010;
byte Input4 = B11000001;
byte CRC = 0;
byte standbyOn = 128;
byte MuteOn = 0;
byte volumeOn = 0;
int buttonPressed = 0;
//boundary voltage range for 10 bit ADC input for control buttons
int inputPressed[2] = {224, 248};
int boostPressed[2] = {422, 466};
int gainPressed[2] = {605, 669};
int mutePressed[2] = {785, 867};
byte brightness[15] = {
    1, 1, 2, 3, 4, 6, 8, 11, 16, 23, 32, 45, 64, 91, 128};  //log spaced values for Mute LED pulse - used sqrt of 2
int fadeAmount = 1;
byte index = 14;
byte LabelIndex[4] = {
    0,0,0,0};

//You can EDIT these labels to whatever you want. You have 12 spaces maximum.
char inputLabel[10][12] = {
    "CD Player ",
    "Turntable ",
    "MP3 Player",
    "DVD Player",
    "Bluetooth ",
    "Tape Deck ",
    "Radio     ",
    "Television",
    "Computer  ",
    "Smartphone"
};

byte Debug = 0;
boolean goUp = 1;
int vinCheck = 0;  //variable for quick analogread in case of brown-out
int monitored24Vsupply = 0;
unsigned long oldMillis = 0;
unsigned long newMillis = 0;
unsigned long muteMillis = 0;
unsigned long volumeMillis = 0;
unsigned long brownoutMillis = 0;
unsigned long irMillis = 0;

//Our eight 4-byte IR codes
long IR_VolumeUp = 0;
long IR_VolumeUpRepeat = 0;
long IR_VolumeDown = 0;
long IR_VolumeDownRepeat = 0;
long IR_InputUp = 0;
long IR_InputDown = 0;
long IR_Standby = 0;
long IR_Mute = 0;
long IR_Gain = 0;
long IR_Boost = 0;
long IR_Previous = 0;

IRrecv irrecv(IR);
decode_results results;
LiquidCrystal lcd(RS_LCD, EN_LCD, D4_LCD, D5_LCD, D6_LCD, D7_LCD);

#endif
