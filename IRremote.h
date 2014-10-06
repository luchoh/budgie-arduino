//
// File			IRremote.h
// Header
//
// Details		<#details#>
//	
// Project		 First
// Developed with [embedXcode](http://embedXcode.weebly.com)
// 
// Author		Latchezar Hristov
// 				Latchezar Hristov
//
// Date			10/5/14 5:43 PM
// Version		<#version#>
// 
// Copyright	© Latchezar Hristov, 2014
// License    <#license#>
//
// See			ReadMe.txt for references
//


// Core library for code-sense - IDE-based
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(MICRODUINO) // Microduino specific
#include "Arduino.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE

#ifndef IRremote_h
#define IRremote_h



#endif
