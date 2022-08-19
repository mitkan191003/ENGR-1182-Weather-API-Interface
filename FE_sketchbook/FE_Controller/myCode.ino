void myCode()
{
  //----------------------------------------------------------------------------------------
  // This is the tab where student programming is done.
  // A list of available function calls are listed in detail under tab "Student_Functions"
  //
  // NOTE: Always comment your code. Debugging will be quicker and easier to do and will 
  //       especially aid the instructional team in helping you. 
  //
  // IF YOU ARE RECORDING DATA TO EEPROM:
  // (2) DO NOT turn the controller off, connect/disconnect the controller to a computer, or 
  //     push the reset button on the Arduino until your program has run to completion and
  //     the indicator LED has stopped flashing. There is a 13 second processing period when
  //     data is stored and battery recuperation takes place. 
  // (2) Data will be recorded approximately every 100 milliseconds. This may vary depending 
  //     on code complexity & operational tasks of your prototype. 
  //---------------------------------------------------------------------------------------- 

  // Program between here-------------------------------------------------------------------
  pinMode(IO_PORT3, INPUT); //Trigger from ESP8266
  while (true) {
    if(digitalRead(IO_PORT3) == 1) {
      for (int i = 0; i < 5; i++) {
        rotateServo(0);
        pauseFor(1);
        rotateServo(55);
        pauseFor(1);
      }
    }
  }
  
  // And here--------------------------------------------------------------------------------

//EndMyCode(); //Comment out this function to loop myCode
} // DO NOT REMOVE. end of void myCode()
