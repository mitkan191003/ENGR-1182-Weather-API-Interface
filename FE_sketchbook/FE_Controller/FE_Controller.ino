
/* DO NOT EDIT THIS PAGE - DO NOT EDIT THIS PAGE - DO NOT EDIT THIS PAGE
 * DO NOT EDIT THIS PAGE - DO NOT EDIT THIS PAGE - DO NOT EDIT THIS PAGE
 * DO NOT EDIT THIS PAGE - DO NOT EDIT THIS PAGE - DO NOT EDIT THIS PAGE
=========================================================================================
FE_Sketchbook version 2021.01
 
 - A list of available function calls are listed in detail under tab "Student_Functions".
 - Student programming is done under tab "myCode".
 - NOTE: This software is open source. Any modifications made by the user to subroutines 
 is at the risk of the user. 
 
=========================================================================================
*/

// ---------------------------------------------------------------------------
// Include Libraries 
// ---------------------------------------------------------------------------
#include <EEPROM.h>      // Writes to internal 1K byte eeprom storage
#include <Wire.h>        // Writes to external 32K byte external eeprom  
#include <EEPROMI2C.h>   // Writes to external 32K byte external eeprom
#include <Metro.h>       // use metro for timing events
#include <Servo.h>       // use servo library -for servo operations
#include <FE_lib.h>     //custom FE Arduino Controller Library
//#include <FE_encoders.h> //custom FE library for using D2 and D3 as encoders

// ---------------------------------------------------------------------------
// Enter setup
// ---------------------------------------------------------------------------
void setup()   
{ 
  // --------------------------------------------------------------------------
  // Pre-Run Subroutine
  Serial.println("\nPRE PROCESS");
  preProcess();
  // --------------------------------------------------------------------------
}

void loop() {
  // -------------------------------------------------------------------------
  // Execute myCode
  Serial.println("\nRUNNING MYCODE()");
  myCode();
  pauseFor(.01);
  // -------------------------------------------------------------------------
  
  if (startButtonPressed()==1) postProcess(); //stop loop if start button pressed
  }
