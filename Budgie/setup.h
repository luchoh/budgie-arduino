void setup() {
    
    //sets up the SIPO shift register (TPIC6C595) control lines
    pinMode(ENABLE_595, OUTPUT);
    pinMode(CLK_595, OUTPUT);
    pinMode(LATCH_595, OUTPUT);
    pinMode(DATA_595, OUTPUT);
    
    //turns on our LCD backlight
    pinMode(BACKLIGHT_LCD, OUTPUT);
    digitalWrite(BACKLIGHT_LCD, HIGH);
    
    //turns on our power led
    pinMode(POWER_LED, OUTPUT);
    //possible noise in left channel if PWM
    //analogWrite(POWER_LED, 128);
    digitalWrite(POWER_LED, HIGH);
    
    //turns on IR input
    pinMode(IR, INPUT);
    
    //sets up the SN754410 H-bridge control lines
    pinMode(VOL_2A, OUTPUT);
    pinMode(VOL_1A, OUTPUT);
    pinMode(VOL_EN, OUTPUT);
    
    //sets up our pushbutton switch analog array
    pinMode(BUTTONS, INPUT_PULLUP);
    
    //sets up our voltage monitoring of VIN at the proportional filament voltage tap
    //to quickly mute output on power off event
    pinMode(BROWNOUT, INPUT);
    
    //this delay lets all the caps charge up before the filaments are engaged
    delay(5);
    
    //turns on 24V to tubes so they can start warming up and puts the
    //register in a known state to prevent relay clacking
    digitalWrite(LATCH_595, LOW);
    shiftOut(DATA_595, CLK_595, MSBFIRST, Status);
    digitalWrite(LATCH_595, HIGH);
    
    //now that we have known data latched into the TPIC6C595 we enable
    //the output of the shift register for the remainder of the session
    digitalWrite(ENABLE_595, LOW);
    //this stutter lets the filaments preheat some before fully engaging 24V
    //it is meant as a inrush current technique to prevent PS fromfoldback
    delay(100);
    digitalWrite(ENABLE_595, HIGH);
    delay(100);
    digitalWrite(ENABLE_595, LOW);
    
    //serial set up for debugging purposes
    Serial.begin(9600);
    
    //EEPROM: Reads and Writes
    //I use EEPROM.write for writing one byte values and EEPROM_writeAnything for multiple byte variables
    //I use EEPROM_readAnything for all EEPROM reads as I like the easy syntax
    
    //first time program runs we setup our default EEPROM data
    //after initial Nano programming EEPROM will be all 0xFF including the CRC byte
    EEPROM_readAnything(5, CRC);
    
    if (CRC != 90){
        EEPROM.write(0, 1);    // defaults to Input 1
        EEPROM.write(1, Input1);  // default Input 1 settings
        EEPROM.write(2, Input2);  // default Input 2 settings
        EEPROM.write(3, Input3);  // default Input 3 settings
        EEPROM.write(4, Input4);  // default Input 4 settings
        EEPROM.write(5, 90);   //the CRC byte, pattern is B01010101
        //initializes the default labels, i.e. byte LabelIndex[4] = {0,1,2,3};
        EEPROM.write(6, 0);  // default Input 1 label
        EEPROM.write(7, 1);  // default Input 2 label
        EEPROM.write(8, 2);  // default Input 3 label
        EEPROM.write(9, 3);  // default Input 4 label
        EEPROM.write(10, Debug);  // default debug flag
        //these are the default IR codes that get written to the EEPROM from an Apple Remote
        //you can hard code your own remote's codes here if you know them
        EEPROM_writeAnything(50, 0x77E1D0E2);  //IR_VolumeUp
        EEPROM_writeAnything(54, 0xFFFFFFFF);  //IR_VolumeUpRepeat
        EEPROM_writeAnything(58, 0x77E1B0E2);  //IR_VolumeDown
        EEPROM_writeAnything(62, 0xFFFFFFFF);  //IR_VolumeDownRepeat
        EEPROM_writeAnything(66, 0x77E1E0E2);  //IR_InputUp
        EEPROM_writeAnything(70, 0x77E110E2);  //IR_InputDown
        EEPROM_writeAnything(74, 0x77E1BAE2);  //IR_Standby
        EEPROM_writeAnything(78, 0x77E17AE2);  //IR_Mute
        EEPROM_writeAnything(82, 0x77777777);  //IR_Gain - dummy data and not used with the Apple remote
        EEPROM_writeAnything(86, 0x77E140E2);  //IR_Boost
    } /* if */
    
    //read EEPROM data into RAM
    EEPROM_readAnything(0, StatusAddress);
    EEPROM_readAnything(1, Input1);
    EEPROM_readAnything(2, Input2);
    EEPROM_readAnything(3, Input3);
    EEPROM_readAnything(4, Input4);
    EEPROM_readAnything(6, LabelIndex[0]);
    EEPROM_readAnything(7, LabelIndex[1]);
    EEPROM_readAnything(8, LabelIndex[2]);
    EEPROM_readAnything(9, LabelIndex[3]);
    EEPROM_readAnything(10, Debug);
    EEPROM_readAnything(50, IR_VolumeUp);
    EEPROM_readAnything(54, IR_VolumeUpRepeat);
    EEPROM_readAnything(58, IR_VolumeDown);
    EEPROM_readAnything(62, IR_VolumeDownRepeat);
    EEPROM_readAnything(66, IR_InputUp);
    EEPROM_readAnything(70, IR_InputDown);
    EEPROM_readAnything(74, IR_Standby);
    EEPROM_readAnything(78, IR_Mute);
    EEPROM_readAnything(82, IR_Gain);
    EEPROM_readAnything(86, IR_Boost);
    
    /* start debug code */
    //this outputs all the EEPROM data to the Serial Monitor for debugging
    Serial.println("EEPROM Load from Setup.h");
    Serial.println(StatusAddress, DEC);
    Serial.println(Input1, BIN);
    Serial.println(Input2, BIN);
    Serial.println(Input3, BIN);
    Serial.println(Input4, BIN);
    Serial.println(IR_VolumeUp, HEX);
    Serial.println(IR_VolumeUpRepeat, HEX);
    Serial.println(IR_VolumeDown, HEX);
    Serial.println(IR_VolumeDownRepeat, HEX);
    Serial.println(IR_InputUp, HEX);
    Serial.println(IR_InputDown, HEX);
    Serial.println(IR_Standby, HEX);
    Serial.println(IR_Mute, HEX);
    Serial.println(IR_Gain, HEX);
    Serial.println(IR_Boost, HEX);
    /* end debug code */
    
    //set up the LCD, splash screens and tube delay
    Setup_LCD();
    
    //IR pin initialization
    irrecv.enableIRIn(); // Start the receiver
    
    //load last known setting
    switch (StatusAddress)  {
        case 1:
            Status = Input1;
            break;
        case 2:
            Status = Input2;
            break;
        case 3:
            Status = Input3;
            break;
        case 4:
            Status = Input4;
            break;
    } /* switch */
    
    /* vinCheck monitors our 24V supply from a tap off the filament supply.
     We monitor this voltage every two seconds in our void main() and see
     if it has dipped by 2%. That would indicate that the power has been
     switched off, and we immediately mute the output to prevent a large
     thump which can damage speakers. This ciruit will also let us know if tubes
     have been inserted. It will warn the user and mute the output. */
    vinCheck = analogRead(BROWNOUT);
    vinCheck = vinCheck * .98;
    
    //update relays
    digitalWrite(LATCH_595, LOW);
    shiftOut(DATA_595, CLK_595, MSBFIRST, Status);  
    digitalWrite(LATCH_595, HIGH);
    
    //update LCD
    UpdateLCD();
    
} /*setup*/