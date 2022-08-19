//=============================================================================================================
//Useful Functions:
//=============================================================================================================
//  (a) pinMode()
//  (b) digitalRead()
//  (c) digitalWrite()
//  (d) analogRead()
//  (e) analogWrite()
//  (f) pauseFor()
//  (g) Serial.print()
//  (h) rotateServo()
//  (i) motorSpeed()
//  (j) reflectanceSensorTest()
//  (k) getTotalCounts()
//  (l) getForwardCounts()
//  (m) getBackwardCounts()
//  (n) getEncoderDirection()
//  (o) recordDigitalSensor()
//  (p) recordAnalogSensor()
//  (q) startDataRecord()
//=============================================================================================================   
//   KEYWORD DETAILS:
//=============================================================================================================   
//**********************************************************************************************************
//  (a) pinMode(port, mode) 
//        Configures the specified port to behave either as an input or an output.
//
//        port: the FE Controller port number being used (IO_PORT1 – IO_PORT4)
//        mode: INPUT for FE Controller sensors 
//
//          example call: 
//              pinMode(IO_PORT1, OUTPUT); //Establishes port 1 as a digital output
//              pinMode(IO_PORT2, INPUT);  //Establishes port 2 as a digital input
//**********************************************************************************************************
//
//**********************************************************************************************************
//  (b) digitalRead(port)
//        Reads the value from a specified digital port, returns either HIGH or LOW
//
//        port: the FE Controller port you want to read (IO_PORT1 – IO_PORT4)
//
//        example call:
//            sensorVal = digitalRead(IO_PORT2); //reads the digital sensor connected to port 2 
//                                                  and assigns the value to variable senorVal
//**********************************************************************************************************
//
//**********************************************************************************************************
//  (c) digitalWrite(port,value)
//        Writes the value (HIGH or LOW) to a specified digital port
//  
//        port: the FE Controller port you want to write to (IO_PORT1 – IO_PORT4) 
//      
//        example call:
//            digitalWrite(IO_PORT1, HIGH); //sets the output connected to port 1 to HIGH
//**********************************************************************************************************
//
//**********************************************************************************************************
//  (d) analogRead(port)
//         Reads the value from the specified analog port, returns integer between 0 and 1023.
//
//         port: the FE Controller port you want to read (IO_PORT1 or IO_PORT2 for FE Controller)
//
//         example call:
//             sensorVal = analogRead(IO_PORT1)  //reads the analog sensor connected to port 1 
//                                                  and assigns the value to variable senorVal
//**********************************************************************************************************
//
//**********************************************************************************************************
//  (e) analogWrite(port,value)
//        Writes the value (integer between 0 and 255) to specified analog port
//
//        port: the FE Controller port you want to write to (IO_PORT1 or IO_PORT2 for FE Controller) 
//
//        example call:
//            analogWrite(IO_PORT2,125) //set the analog output connected to port 2 to a value of 125.
//**********************************************************************************************************
//
//**********************************************************************************************************
//  (f) pauseFor(t); --> 1 argument
//          t = time in seconds to pause
//
//          
//          example call:
//            digitalWrite(IO_PORT1, HIGH); //sets output connected to IO_PORT1 to HIGH
//            pauseFor(5);  //waits for 5 seconds
//            digitalWrite(IO_PORT1, LOW); //sets output connected to IO_PORT1 to LOW
//**********************************************************************************************************
//
//**********************************************************************************************************
//
//**********************************************************************************************************
//  (g) Serial.print(val)
//        Prints data to the Arduino IDE serial monitor as human-readable ASCII text
//          example call:
//            rotateServo(10.00);   // Initialize servo and set to 10 degrees
//            rotateServo(170.00);  // Rotate servo to 170 degrees
//            
//            Note: The limits, 0 & 180 degrees, are not nesecarily what will you see when you set your servo
//                  to these values. The Tower Pro SG92R servos will get close to 0 or 180 degrees but not exact.
//**********************************************************************************************************
//
//**********************************************************************************************************
//  (j) reflectanceSensorTest(); --> no arguments - prints total counts, forward counts, backwards counts and 
//                                                  direction of rotation to serial monitor
//          
//          example call:
//            reflectanceSensorTest();  //Test operation of reflectance sensors.
//            (note: Serial Monitor will display the total counts, forward counts, backwards counts and direction of rotation)
//**********************************************************************************************************
//   
//**********************************************************************************************************
//   (k) getTotalCounts(); --> no arguments - returns total number of encoder counts detected.
//   
//          example call:
//            int totalMarks = getTotalCounts();
//            Note: If you travel 50 marks forwards and 20 marks backwards, then the number returned
//            from this function will be 70. In this case, totalMarks = 70.
//**********************************************************************************************************
//
//**********************************************************************************************************
//  (l) getForwardCounts(); --> no arguments - Returns total number of encoder counts detected in the forward 
//                                              direction.
//   
//          example call:
//            int fMarks = getForwardCounts();
//            Note: If you travel 50 marks forwards and 20 marks backwards, then the number returned
//            from this function will be 50. In this case, fMarks = 50.
//**********************************************************************************************************
// 
//**********************************************************************************************************
//  (m) getBackwardCounts(); --> no arguments - returns total number of encoder counts detected in the backward 
//                                              direction.
//   
//          example call:
//            int bMarks = getBackwardCounts();
//            Note: If you travel 50 marks forwards and 20 marks backwards, then the number returned
//            from this function will be 20. In this case, bMarks = 20.
//**********************************************************************************************************
//   
//**********************************************************************************************************
//  (n) getEncoderDirection(); --> no arguments - returns direction of rotation detected by encoder.
//   
//          example call:
//            int vehicleDir = getEncoderDirection(); // Check direction.
//            Note: If vehicleDir == 1, vehicle is moving forward. 
//                  If vehicleDir == 0, vehicle is moving in reverse.
//                  If vehicleDir == 2, no direction detected.
//**********************************************************************************************************
//
//**************************************************************************
//  (o) recordDigitalSensor(port)
//         specifies digital sensor port to record to EEPROM
//              
//         port: IO_PORT1 - IO_PORT4
//
//          example call:
//              recordDigitalSensor(IO_PORT4); //sensor connected at IO_PORT4 will now record to EEPROM
//**************************************************************************
//
//**************************************************************************
//  (p) recordAnalogSensor(port)
//          specifies analog sensor port to record to EEPROM
//              
//          port: IO_PORT1, IO_PORT2, or ENCODERS
//
//          example call:
//              recordAnalogSensor(IO_PORT1); //sensor connected at IO-PORT1 will now record to EEPROM
//              recordAnalogSensor(ENCODERS); //FWD, BWD, and Total counts will now record to EEPROM
//**************************************************************************
//
//**************************************************************************
//  (q) startDataRecord() --> starts recording data to EEPROM
//        NOTE: Only call this function AFTER specifying all sensor pins to record
//
//          example call:
//              recordAnalogSensor(IO_PORT1); //sensor connected at IO_PORT1 will now record to EEPROM
//              recordDigitalSensor(IO_PORT3); //sensor connected at IO_PORT3 will now record to EEPROM
//              startDataRecord(); //Data will begin recording to EEPROM approx. every 60ms
//
//************************************************************************** 
