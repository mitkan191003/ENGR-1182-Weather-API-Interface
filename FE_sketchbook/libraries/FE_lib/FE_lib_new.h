//FE Controller Custom Library
#ifndef FE_LIB_H
#define FE_LIB_H

// ---------------------------------------------------------------------------
// Pin definitions
// ---------------------------------------------------------------------------
const byte pwmPin1       =  5; // Motor 1 pwm pin
const byte hblogic1a     =  7; // Motor 1 direction pin A
const byte hblogic1b     =  8; // Motor 1 direction pin B
const byte pwmPin2       =  6; // Motor 2 pwm pin
const byte hblogic2a     = 16; // Motor 2 direction pin A
const byte hblogic2b     = 17; // Motor 2 direction pin B
const byte pwmPin3       = 11; // Motor 3 pwm pin (optional)
const byte hblogic3a     = 10; // Motor 3 direction pin A
const byte hblogic3b     = 12; // Motor 3 direction pin B
const byte servoPin      =  9; // Servo pin declaration
const byte buttonBypass  =  4; // Program start button
const byte curSensePin   = A7; // Current is sensed on analog 7
const byte batVoltsPin   = A6; // Voltage is sensed on analog 6
const byte yellowLedPin  = 13; // Pretty yellow LED, IT'S NOT AN ARDUINO WITHOUT AN LED!

// ---------------------------------------------------------------------------
// Variable Declarations 
// ---------------------------------------------------------------------------

// Motor speed power adjustment
int directionStatus                  = 1;         // initialize motor direction; 0 is reverse
int previousSpeed                    = 0;         // Initialize previous motor speed (%).
int accDccTimeAdjustment             = 0;         // Initialize Acceleration Time Adjustment (second).

// Voltage & Current Reference
const float extRef                   = 2.46;      // External reference voltage
const float defRef                   = 5.0;       // Default reference voltage
const float lowVolts                 = 6.75;      // Minimum voltage required for operation
int cVolt1                           = 0;         // Initiate the no motor(s) running current variable

// Timing
Metro metroTimer                     = Metro();   // Initialize metro timer
unsigned long startTime              =    0;      // Reference for actual start time
unsigned long readTime = 0;
unsigned long lastRecordedTime       =    0;      // Used for ensuring record times to do not occur under "minTimeLapse"
unsigned long minTimeLapse           =   100;     // Threshold for recording data (milliseconds) 
int timer2Count_dataRecord 			 =    0;	  //Used to count calls for recordData Timer2 interrupt

// EEprom storage
unsigned int nBytes                 = 0;         // Number of bytes EEProm recorded
int first                            = 1;         // Determine whether it is the first time recording data.
uint32_t timez                       = 0;         // Initialize time for the data collection.

// FE Servo
Servo FEServo;

// Data collection variables
unsigned long theTime=0;;   // Time recorded, in milliseconds
int cVolts=0;               // current counts
int bVolts=0;               // voltage counts
unsigned int tMarks=0;      // Total marks (Cumulative Count)
uint16_t fMarks=0;          // Total forward marks
uint16_t bMarks=0;          // Total backward marks


const byte EncoderIndex = 0;
const byte D10Index = 1;
const byte D14Index = 2;
const byte D15Index = 3;
const byte A0Index = 4;
const byte A1Index = 5;
const byte ENCODERS = 201;

bool Data2Record[6] = {0}; //[Encoder Counts, D10, D14, D15, A0, A1]
bool startRecording = 0; 
bool usingEncoders = 0; //trigger if using encoder library

// ---------------------------------------------------------------------------

//----------------------------------------------------------------------------
//-------------------------- HIGH LEVEL FUNCTIONS ----------------------------
//----------------------------------------------------------------------------


//******************************************************************************************
// getSamples

int getSamples(byte pin, int numOfSamples, char type)
{
  //--------------------------------------------------------------
  // getSamples(pin, numOfSamples, type);
  //
  // Returns voltage or current counts.
  //
  // Input: pin          : Pin to read from.
  //        numOfSamples : Number of samples to take.
  //        type('e'/'d'): String specifying type of reading.   
  //
  // Output: valueOut    : Sampled voltage or current counts
  //--------------------------------------------------------------
  
  // Determine analog type
  if (type=='e') // Take current reading
  {
    analogReference(EXTERNAL); // When taking current data us EXTERNAL (2.46 volts)
  }
  else // Take voltage reading
  {
    analogReference(DEFAULT); // When taking voltage data use DEFAULT (5.00 volts)
  }
  
  // Initialize average value
  long sampleAvg=0; 
  
  // get numOfSamples samples
  for (byte j = 0; j < (numOfSamples + 2); j++)
  {
    // oddly, the first two values are considerably off 
    if (j < 2)
    {
      analogRead(pin); // Throw away value
    }
    else 
    {
      // Cumulative sum
      sampleAvg = sampleAvg + analogRead(pin);
    }
  }
      
  //  Compute average output
  int valueOut = (int)(sampleAvg/numOfSamples);
  
  // Post process current reading
  if (type=='e') // Take current reading
  {
    // Post process current count
    valueOut = valueOut - cVolt1;
    valueOut = (valueOut < 1) ? 0 : valueOut;
  }
  
  return valueOut;       
}
//******************************************************************************************

//******************************************************************************************
// timeNow 
unsigned long timeNow()
{
  //----------------------------------------------------------------------------------------
  // timeNow();
  //
  // Returns current time referenced from the start time of the program
  //
  // Input: none
  //
  // Output: currentTime
  //---------------------------------------------------------------------------------------- 

  unsigned long currentTime = (millis()-startTime);
  
  return currentTime;
}
//******************************************************************************************

//******************************************************************************************
//recordData
void recordData()
{
  //----------------------------------------------------------------------------------------
  // recordData();
  //
  // Record time,current,voltage,total marks and postion to external eeprom
  //
  // Input: none
  //
  // Output: none
  //
  // Data Recording information:
  //   The 32K eeprom can record 4096 data sets (4 integers in a set, 2 bytes per integer 
  //   or 32768/8). What's this telling us? Well, for example for a maximum length run of 
  //   120 seconds then only 34 sets per second are allowed or approximately 1 every 30 ms 
  //   Currently 100 ms is the time lapse.
  //---------------------------------------------------------------------------------------- 

  // delay time (ms) between consecutive writes to external EEPROM
  int del_time=8;
  // Get time
  //theTime = timeNow();
  
  // Get current counts
  cVolts=getSamples(curSensePin,200,'e');
     
  // Get voltage counts       
  bVolts=getSamples(batVoltsPin,100,'d');

  //Get encoder counts
  #ifdef FE_ENCODERS_H //if we are using the encoders library
	tMarks=getTotalCounts();
	fMarks=getForwardCounts();
	bMarks=getBackwardCounts();
  #else //if we are not using the encoders library
	tMarks=0;
	fMarks=0;
	bMarks=0;
  #endif

  //variable to read in sensor values
  int sensorVal=0;
     
  // Write data to EEprom  
  
  //Time
  eeWrite(nBytes, theTime);
  //delay(del_time);

  nBytes = nBytes + (sizeof(theTime));
  
//delay(del_time);

  //current
  eeWrite(nBytes, cVolts);
  //delay(del_time);

  nBytes = nBytes + (sizeof(cVolts));

  
  //battery voltage
  eeWrite(nBytes, bVolts);
 // delay(del_time);

  nBytes = nBytes + (sizeof(bVolts));

if (Data2Record[EncoderIndex]) {
  //Total Encoder Counts
  eeWrite(nBytes, tMarks);
  //delay(del_time);

  nBytes = nBytes + (sizeof(tMarks));


  //Forward Encoder Counts
  eeWrite(nBytes, fMarks);
  //delay(del_time);

  nBytes = nBytes + (sizeof(fMarks));


  //Backward Encoder Counts
  eeWrite(nBytes, bMarks);
  //delay(del_time);

  nBytes = nBytes + (sizeof(bMarks));
}

if (Data2Record[D10Index]) {
  //D10 Sensor input
  sensorVal=digitalRead(10);
  eeWrite(nBytes, sensorVal);
  //delay(del_time);

  nBytes = nBytes + (sizeof(sensorVal));
}

if (Data2Record[D14Index]) {
  //D14 Sensor input
  sensorVal=digitalRead(14);
  eeWrite(nBytes, sensorVal);
  //delay(del_time);

  nBytes = nBytes + (sizeof(sensorVal));
}

if (Data2Record[D15Index]) {
  //D15 Sensor input
  sensorVal=digitalRead(15);
  eeWrite(nBytes, sensorVal);
  //delay(del_time);

  nBytes = nBytes + (sizeof(sensorVal));
}

if (Data2Record[A0Index]) {
  //A0 Sensor input
  sensorVal=analogRead(A0);
  eeWrite(nBytes, sensorVal);
  //delay(del_time);

  nBytes = nBytes + (sizeof(sensorVal));
}

if (Data2Record[A1Index]) {
  //A1 Sensor input
  sensorVal=analogRead(A1);
  eeWrite(nBytes, sensorVal);
  //delay(del_time);

  nBytes = nBytes + (sizeof(sensorVal));
}

}
//******************************************************************************************

//******************************************************************************************
// ledFlash

void ledFlash(byte numOfTimes, int flashDuration, int timeBetweenFlashes)
{
  //------------------------------------------------------------------------
  // ledFlash(numOfTimes, flashDuration, timeBetweenFlashes);
  //
  // Flashes Arduino LED a sepcified number of times.
  //
  // Input: numOfTimes        : Number of times to flash LED.
  //        flashDuration     : Length of time to keep LED on.
  //        timeBetweenFlashes: Time between flashes.
  //
  // Output: None
  //-------------------------------------------------------------------------
  
  // Turn LED off
  digitalWrite(yellowLedPin,LOW);
      
  // Enter while loop
  do
  { 
    // Turn LED on
    digitalWrite(yellowLedPin,HIGH);
        
    // Keep LED on for ''flashduration''
    delay(flashDuration);

    // Turn LED off
    digitalWrite(yellowLedPin,LOW);

    // Keep LED on for ''flashduration''
    delay(timeBetweenFlashes);
    
    // Decrease number of iterations
    numOfTimes--;
    
  } 
  while (numOfTimes>0);
}
//******************************************************************************************


//**************************************************************************
//--------------------------------------------------------------------------
// Matlab <-> Arduino Communication Library
//
// Consist of a library of routines that are used in data transfer. 
// The following subroutines are contained in this library:
// 
// (1) establishContact
// (2) sendArduino2Matlab
//
//--------------------------------------------------------------------------

// (1) Establish contact with Matlab****************************************

boolean establishContact() 
{    
  //------------------------------------------------------------------------
  // establishContact();
  //
  // Send 1's to serial port for establishing constact with Matlab.
  //
  // Input: None
  //
  // Output: None
  //-------------------------------------------------------------------------
  
  // Initialize flag
  boolean flag=false;
  
  // Set timer flag
  unsigned long timer1 = millis();
  
  // Check connection for 500 milliseconds
  while (millis()-timer1 < 500)  
  {
    if (Serial.available() <= 0) // If serial is not available
    {
      Serial.println(1, DEC);    // Try sending a number
      delay(50);
    }
    else                         // else, we're connected! 
    {
      flag=true;
      break;
    }
  }
  return flag;
}
//**************************************************************************

// Send arduino data to Matlab via serial communication*********************

void sendArduino2Matlab()
{ 
  //------------------------------------------------------------------------
  // sendArduino2Matlab();
  //
  // Transfer data from Arduino to Matlab.
  //
  // Input: None
  //
  // Output: None
  //-------------------------------------------------------------------------
  
  
  // Get total number of bytes stored in eeprom
  byte a = EEPROM.read(0);
  byte b = EEPROM.read(1);
  int total_bytes=word(a,b);

  // Send total number of bytes for Matlab to read
  Serial.println(total_bytes); 
  
  int nelements = 3; //counts number of elements recorded

// Recall and send which optional data was recorded 
for (int index=0; index < (sizeof(Data2Record)/sizeof(Data2Record[0])) ; index++) 
  {
    Data2Record[index] = EEPROM.read(index+2);
    Serial.println(Data2Record[index]);
    if (index == EncoderIndex) {
	  if(usingEncoders) nelements += Data2Record[index]*3;
	}
    else {
	nelements += Data2Record[index];
	}
  }

  // Send data stored as bytes in external eeprom
  // time, current, voltage, total marks, forward marks, backward marks
  int j=0; //counts bytes read from EEProm
  int data_counter=1; //counts data value to read from EEProm
  
  while (j<total_bytes)
  {    
        int intVal;
	unsigned long longVal;
        int del_time = 1;
	
	//checks if time is being read and stores as float
	if ((data_counter % nelements) == 1 ) {
		eeRead(j,longVal);
		delay(del_time);
                Serial.println(longVal);
		j = j + sizeof(longVal);
	}
	
	//all values other than time, stored as int
    else {
		eeRead(j,intVal);
       		delay(del_time);
    	        Serial.println(intVal);
    	        j = j + sizeof(intVal);
	}
	
	data_counter++;
  }

  
}
//**************************************************************************



//----------------MOTOR FUNCTIONS-------------------------------------------
//--------------------------------------------------------------



//**************************************************************************
// Instantaneous Motor Speed (NO Acceleration/Deceleration)
void motorSpeedAD(byte motor, byte percentSpeed)
{
  //--------------------------------------------------------------
  // motorSpeed(motor, percentSpeed);
  //
  // Initialize motor at percentSpeed.
  //
  // Input: motor        : Motor Number (1-3, 4 = All Motors).
  //        percentSpeed : Percent power. 0-100%
  //
  // Output: none
  //--------------------------------------------------------------
  
  // Check motor input. If incorrect, display error in serial port for user.
  if(motor < 1) motor = 1;
  if(motor > 4) motor = 4;
  
  // Computer % speed without power adjustment
  percentSpeed = percentSpeed;
  
  // Check percentSpeed input.
  if(percentSpeed < 0)   percentSpeed = 0;
  if(percentSpeed > 100) percentSpeed = 100;
   
  // Map out the percent speed provided by the user
  byte s = map(percentSpeed,0,100,0,255); 

  // Determine which motor to apply power to.
  switch (motor) 
  {   
    
  // Initializes motor 1.  
  case 1:

    analogWrite(pwmPin1,s); break;
    
  // Initializes motor 2.  
  case 2:

    analogWrite(pwmPin2,s); break;
    
  // Initializes motor 3. 
  case 3:

    analogWrite(pwmPin3,s); break;
  
  // Initializes all motors.      
  default: 

    analogWrite(pwmPin1,s); // motor 1
    analogWrite(pwmPin2,s); // motor 2
    analogWrite(pwmPin3,s); // motor 3
    
  }
}
//**************************************************************************

//**************************************************************************
// Acceleration and Deceleration Function
void celerate(byte motor, byte startSpeed, byte endSpeed, float numOfSeconds)
{
  //----------------------------------------------------------------------------------------
  // celerate(motor, startSpeed, endSpeed, seconds);
  //
  // Accelerate/Decelerate motor(s) from startSpeed to endSpeed in
  // numOfSeconds seconds.
  //
  // Input: motor        : Motor Number (1-3, 4 = All Motors).
  //        startSpeed   : Percent power. 0-100%
  //        endSpeed     : Percent power. 0-100%  
  //        numOfSeconds : Time frame to go from startSpeed-endSpeed
  //
  // Output: none
  //---------------------------------------------------------------------------------------- 
    
  // Enforce limits------------------------------------------------------------------------- 
  
  // Starting & ending power constraints
  if (startSpeed<0)   startSpeed=0;
  if (startSpeed>100) startSpeed=100;
  if (endSpeed<0)     endSpeed=0;
  if (endSpeed>100)   endSpeed=100;
  
  // Time period constraints
  //if (numOfSeconds < 1.0)   numOfSeconds = 1.0;
  //if (numOfSeconds > 10.0)  numOfSeconds = 10.0;
  
  // Motor constraints
  if (motor<1) motor=1;
  if (motor>4) motor=4;
  
  //----------------------------------------------------------------------------------------
  
  // Set up timing variables----------------------------------------------------------------
  
  // Compute metro timer input in milliseconds
  unsigned long numOfMillis = numOfSeconds*1000;
  
  // Compute timestep 
  unsigned long tStep = (numOfMillis)/abs(startSpeed - endSpeed);

  // Initialize motor step (for accelerating or decelerating)
  int stepper = (startSpeed > endSpeed) ? -1 : 1;
  
  // Reset metro timer 
  metroTimer.interval(numOfMillis); // Start timer 
  metroTimer.reset();               // Reset metro timer 
  
  // Initialize timing flag
  unsigned long t1 = millis();

  //----------------------------------------------------------------------------------------
  
  // Set initial motor speeds---------------------------------------------------------------
  byte celerateSpeed = startSpeed;
  motorSpeedAD(motor,celerateSpeed);
  //----------------------------------------------------------------------------------------
    
  // Enter timing loop----------------------------------------------------------------------
  do
  {
    // Record data
 //   recordData();
    
    if(((millis() - t1) >= tStep)) // If the difference in time lapse is g.t.e. to our time step
    {
      t1 = millis(); // Reassign timing flag  
     
      celerateSpeed = celerateSpeed + stepper; // Accelerate motor to next step
      
      // Set motor speeds
      motorSpeedAD(motor,celerateSpeed);      
    }
    else
    {
      continue; 
    }
  }
  while (!metroTimer.check()==1);
  
  // Set final motor speeds--------------------------------------------------------
  // Enforce ending speed (The time step equation works perfectly without the 
  // "recordData();" function call. Since we need to use "recordData();" this takes 
  // up ~36-40 milliseconds every ~60 milliseconds it records data. So the ending 
  // speed may be a few short. Simply enforce the ending speed.  
  motorSpeedAD(motor,endSpeed);
  //----------------------------------------------------------------

}
//--------------------------------------------------------------------
//**************************************************************************

//**************************************************************************
// Reverse Function
void reverse(byte motor)
{
  //--------------------------------------------------------------
  // revers(motor);
  //
  // Reverse direction of motor.
  //
  // Input: motor: Motor Number (1-3, 4 = All Motors).
  //
  // Output: none
  //--------------------------------------------------------------
  
  // Check input 
  if(motor < 1) motor = 1;
  if(motor > 4) motor = 4;
    
  // Determine which motor to reverse.
  switch (motor) 
  {   
  
  // Reverse motor 1.      
  case 1:
    
    // Set motor power to 0 before reversing
    analogWrite(pwmPin1,0);
    
    // Reverse motor direction
    digitalWrite(hblogic1a, !digitalRead(hblogic1a) );
    digitalWrite(hblogic1b, !digitalRead(hblogic1b) );
            
    break;
     
  // Reverse motor 2.        
  case 2:
  
    // Set motor power to 0 before reversing
    analogWrite(pwmPin2,0);
    
    // Reverse motor direction
    digitalWrite(hblogic2a, !digitalRead(hblogic2a) );
    digitalWrite(hblogic2b, !digitalRead(hblogic2b) );
  
    break;
      
  // Reverse motor 3.        
  case 3:

    // Set motor power to 0 before reversing
    analogWrite(pwmPin3,0);
    
    // Reverse motor direction
    digitalWrite(hblogic3a, !digitalRead(hblogic3a) );
    digitalWrite(hblogic3b, !digitalRead(hblogic3b) );
    
    break;
      
  // Reverse all motors.       
  default: 
  
    // Set motor power to 0 before reversing
    analogWrite(pwmPin1,0); // motor 1
    analogWrite(pwmPin2,0); // motor 2
    analogWrite(pwmPin3,0); // motor 3
    
    // Reverse motor direction
    digitalWrite(hblogic1a, !digitalRead(hblogic1a) ); // motor 1
    digitalWrite(hblogic1b, !digitalRead(hblogic1b) ); // motor 1
    digitalWrite(hblogic2a, !digitalRead(hblogic2a) ); // motor 2
    digitalWrite(hblogic2b, !digitalRead(hblogic2b) ); // motor 2
    digitalWrite(hblogic3a, !digitalRead(hblogic3a) ); // motor 3
    digitalWrite(hblogic3b, !digitalRead(hblogic3b) ); // motor 3
    
  }
}
//**************************************************************************

//**************************************************************************
// powerAdjust - motorSpeed subroutine
//--------------------------------------------------------------
float powerAdjust()
{
  //--------------------------------------------------------------
  // powerAdjust();
  //
  // Returns a ratio to multiply percentSpeed by to handle battery
  // power reduction as the battery is used.
  //
  // Input: none
  //
  // Output: PowerAdjustment
  //--------------------------------------------------------------
  
  // Get voltage count
  int bVolts=getSamples(batVoltsPin,100,'d');
  
  // Compute battery voltage
  float batVolts = 3.0*defRef*(float)bVolts/1024.0;
  
  // catch a very low battery voltage and stop the run!
  if (batVolts < lowVolts)
  {
    // loop forever if battery voltage is too low
    // (do not allow the run to take place)
	
	motorSpeedAD(4,0); //stop all motors
	
    while(1)
    {
      // set rapid flash if volts to low for run
      ledFlash(1,200,200);
      Serial.print("Run had to stop due to low battery voltage = ");
      Serial.println(batVolts,DEC);
    }
  } 
  
  // Nominal voltage for the Li-Po batteries we use is 7.4 volts
  // Compute ratio to adjust power to get close to actual percent power
  
  float powerAdjustment = 7.4/batVolts;
  
  return powerAdjustment;  
}
//**************************************************************************


//----------------------------------------------------------------------------
//---------------------- AVAILABLE STUDENT FUNCTIONS -------------------------
//----------------------------------------------------------------------------
//=============================================================================================================
//  (a) motorSpeed()
//  (b) pauseFor()
//  (c) rotateServo()
//  (d) getEncoderDirection()
//  (e) getTotalCounts()
//  (f) getForwardCounts()
//  (g) getBackwardCounts()
//  (h) reflectanceSensorTest()
//=============================================================================================================  

//**************************************************************************
// Motor Speed Student Function (Acceleration/Deceleration Built-In
// to prevent large current surge
void motorSpeed(byte motor, int percentSpeed)
{
  //--------------------------------------------------------------
  // motorSpeed(motor, percentSpeed);
  //
  // Initialize motor at percentSpeed.
  //
  // Input: motor        : Motor Number (1-3, 4 = All Motors).
  //        percentSpeed : Percent power. -100% - 100%
  //
  // Output: none
  //--------------------------------------------------------------
  
  // Check motor input. If incorrect, display error in serial port for user.
  if(motor < 1) motor = 1;
  if(motor > 4) motor = 4;
  
  // Computer % speed with power adjustment
  percentSpeed = percentSpeed * powerAdjust();
  
  // Check percentSpeed input.
  if(percentSpeed < -100) percentSpeed = -100;
  if(percentSpeed > 100) percentSpeed = 100;

  // Fix Acceleration/Deceleration Magnitude (0-100% in 1 second or 100-0% in 1 second)
  accDccTimeAdjustment = abs(percentSpeed - previousSpeed)/100; // (100%/s) acceleration/deceleration magnitude.

  // Check Direction
  if (percentSpeed > 0){
    
    // Check the moving direction status of the motor.
    if (directionStatus != 1) {

      if (previousSpeed != 0){
        celerate(motor, abs(previousSpeed), 0, abs(previousSpeed - 0)/100); // Decelerate the motor.

        previousSpeed = 0; // Set previous speed to 0.
      }
       
      reverse(motor); // Reverse the motor.

      directionStatus = 1; // Set the Direction Status to forward.

      }
    }
    
    else if (percentSpeed < 0){
      
      // Check the moving direction status of the motor.
      if (directionStatus != 0) {

        if (previousSpeed != 0){
          
          celerate(motor, abs(previousSpeed), 0, abs(previousSpeed - 0)/100); // Decelerate the motor.

          previousSpeed = 0; // Set previous speed to 0.
        }
        
        reverse(motor); // Reverse the motor.

        directionStatus = 0; // Set the Direction Status to reverse.

      }
    }
    
    else if (percentSpeed == 0){
      
      // Check the moving direction ststus of the motor.
      if (previousSpeed < 0) {
        
        directionStatus = 0; // Set the Direction Status remains reverse.

      }
      else if (previousSpeed > 0){

        directionStatus = 1; // Set the Direction Status remains forward.
      }
    }
  // Determine which motor to apply power to.
  switch (motor) 
  {   
    
  // Initializes motor 1.  
  case 1:

    // Accelerate when necessary
    if (previousSpeed != percentSpeed){
      
      celerate(motor, abs(previousSpeed), abs(percentSpeed), accDccTimeAdjustment); // Accelerate.
      
    }
    
    motorSpeedAD(motor, abs(percentSpeed)); // Set motor speed.
    
    previousSpeed = percentSpeed; // Assign previous speed as current speed.

    break;
    
  // Initializes motor 2.  
  case 2:

    // Accelerate when necessary
    if (previousSpeed != percentSpeed){
      
      celerate(motor, abs(previousSpeed), abs(percentSpeed), accDccTimeAdjustment); // Accelerate.
      
    }
    
    motorSpeedAD(motor, abs(percentSpeed)); // Set motor speed.
    
    previousSpeed = percentSpeed; // Assign previous speed as current speed.

    break;
    
  // Initializes motor 3. 
  case 3:
  
    Serial.println("Motor 3: Not supported"); // Print error message.
  
  // Initializes all motors.      
  default: 

    // Accelerate when necessary
    if (previousSpeed != percentSpeed){
      
      celerate(motor, abs(previousSpeed), abs(percentSpeed), accDccTimeAdjustment); // Accelerate.
      
    }
    motorSpeedAD(motor, abs(percentSpeed)); // Set motor speed.

    // Assign previous speed as current speed. 
    previousSpeed = percentSpeed;
  }
}
//**************************************************************************

//**************************************************************************
// Delay Function
void pauseFor(float numOfSeconds)
{
  //--------------------------------------------------------------
  // pauseFor(numOfSeconds);
  //
  // Continue statement(s) previous to "pauseFor" for "numOfSeconds".
  //
  // Input: numOfSeconds: Time frame, in seconds.
  //
  // Output: none
  //--------------------------------------------------------------
 
  // Compute metro timer input in milliseconds
  unsigned long numOfMillis = numOfSeconds*1000;
  // Reset metro timer 
  metroTimer.interval(numOfMillis);
  metroTimer.reset();
 
  // Enter while loop until time input has passed
  do
  { 
    // Every loop call recordData 
    //recordData();
  } 
  while (!metroTimer.check()==1);
}
//**************************************************************************

//**************************************************************************
// Servo Function
void rotateServo(float angle)
{
  //--------------------------------------------------------------
  // rotateServo(angle);
  //
  // Rotates servo
  //
  // Input: angle        : servo angle (0 - 180 degrees).
  //
  // Output: none
  //--------------------------------------------------------------
  
  // Check motor input. If incorrect, display error in serial port for user.
  if(angle < 0.00) angle = 0.00;
  if(angle > 180.00) angle = 180.00;
   
  // Map out the angle provided by the user
  int ms = map(angle,0.00,180.00,544,2400);
 
  // Rotate servo
  FEServo.writeMicroseconds(ms);
  
}
//**************************************************************************

//**************************************************************************
void recordDigitalSensor(int sensor)
{
  if (sensor == 10) Data2Record[D10Index] = 1; //set to record D10 data in EEPROM
  else if (sensor == 14) Data2Record[D14Index] = 1; //set to record D14 data in EEPROM
  else if (sensor == 15) Data2Record[D15Index] = 1; //set to record D15 data in EEPROM
}
//**************************************************************************

//**************************************************************************
void recordAnalogSensor(int sensor)
{
  if (sensor == A0) Data2Record[A0Index] = 1; //set to record A0 data in EEPROM
  else if (sensor == A1) Data2Record[A1Index] = 1; //set to record A1 data in EEPROM
  else if (sensor == ENCODERS) Data2Record[EncoderIndex]=1;
}
//**************************************************************************

//**************************************************************************
void startDataRecord()
{
  //set timer2 interrupt at 1kHz
  
  cli();//stop interrupts during setup

  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 1khz increments
  OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 64 prescaler
  TCCR2B |= (1 << CS22);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  sei();//allow interrupts
}
//**************************************************************************


ISR(TIMER2_COMPA_vect){  //Timer 2 interrupt function
   timer2Count_dataRecord ++; //increments calls to this function
   if (timer2Count_dataRecord ==100) //only records data at 10 Hz - every 100 calls of this interrupt
   {
    recordData();
    timer2Count_dataRecord =0;
   }
}


//******************************************************************************************
// Pre-process Function
void preProcess()
{
  //------------------------------------------------------------------------
  // preProcess();
  //
  // Pin setup, battery check and wait for user to start run or connect 
  // with Matlab
  //
  // Input: None
  //
  // Output: None
  //-------------------------------------------------------------------------
  
  // Serial is used for Matlab detection-------------------------------------- 
  Serial.begin(115200);
  // -------------------------------------------------------------------------

  // Initiate the Wire library and join the I2C bus--------------------------- 
  Wire.begin(80);
  // -------------------------------------------------------------------------

  // Motor Pin Setup----------------------------------------------------------
  pinMode(pwmPin1,OUTPUT);       // Set Motor 1 mode
  pinMode(hblogic1a,OUTPUT);     // Set Motor 1 direction pin A mode
  pinMode(hblogic1b,OUTPUT);     // Set Motor 1 direction pin B mode
  digitalWrite(hblogic1a,HIGH);  // Set Motor 1 direction pin A HIGH
  digitalWrite(hblogic1b,LOW);   // Set Motor 1 direction pin A LOW

  pinMode(pwmPin2,OUTPUT);       // Set Motor 2 mode
  pinMode(hblogic2a,OUTPUT);     // Set Motor 2 direction pin A mode
  pinMode(hblogic2b,OUTPUT);     // Set Motor 2 direction pin B mode
  digitalWrite(hblogic2a,HIGH);  // Set Motor 2 direction pin A HIGH
  digitalWrite(hblogic2b,LOW);   // Set Motor 2 direction pin A LOW

  pinMode(pwmPin3,OUTPUT);       // Set Motor 3 mode (optional)
  pinMode(hblogic3a,OUTPUT);     // Set Motor 3 direction pin A mode
  pinMode(hblogic3b,OUTPUT);     // Set Motor 3 direction pin B mode
  digitalWrite(hblogic3a,HIGH);  // Set Motor 3 direction pin A HIGH
  digitalWrite(hblogic3b,LOW);   // Set Motor 3 direction pin A LOW
  // --------------------------------------------------------------------------
  
  // Yellow LED Pin & Button Bypass--------------------------------------------
  pinMode(yellowLedPin,OUTPUT);
  digitalWrite(yellowLedPin,LOW);
  pinMode(buttonBypass,INPUT_PULLUP);
  // --------------------------------------------------------------------------
  
  // Quadrature Encoder Setup--------------------------------------------------
  #ifdef FE_ENCODERS_H
	encoderSetup();
  #endif
  // --------------------------------------------------------------------------

  // Servo Pin-----------------------------------------------------------------
  FEServo.attach(9); 
  rotateServo(0); // Initialize servo to 0 degrees
  // --------------------------------------------------------------------------
  
  // Wait for user to start--------------------------------------------------

  digitalWrite(yellowLedPin,HIGH); // Turn on LED 
  
  while (digitalRead(buttonBypass)==HIGH) // While button is not pressed
  {
    if (establishContact()) // Check for contact with Matlab
    {
      sendArduino2Matlab(); // Send Arduino data to Matlab
      while (digitalRead(buttonBypass)==HIGH) {}
      break;
    }
  } 
  //-------------------------------------------------------------------------
  
  //-------------------------------------------------------------------------
  
  // Prepare to start run----------------------------------------------------
  
  // Give 4 second delay until start - Flash LED 4 times
  ledFlash(4,500,500);   
                     
  
  // Clear first 10 bytes of internal eeprom
  for(byte q=0;q<10;q++) EEPROM.write(q,0);   
  
  // Do a base current usage check with all motors braked
  motorSpeedAD(4,0); cVolt1 = getSamples(curSensePin,200,'e');
  
  // Get Actual start time
  startTime      = millis();
  
}
//******************************************************************************************

//******************************************************************************************
// Post Process Function
void postProcess()
{
  
  //unsigned int eepromAddress = 0;  // eeprom address 
  //byte rl                    = 2;  // Starting address to store bat life to int eeprom

   // Run is over stop all motors
  motorSpeedAD(4,0);
  
  // Collect data for an additional 3 seconds
  pauseFor(3);
  
  //turn off data record interrupt on Timer2
  TIMSK2 &= ~(1 << OCIE2A); 
  
  // shut everything down
  // store total number of data bytes collected to eeprom
  EEPROM.write(0, highByte((int) nBytes)); delay(5);
  EEPROM.write(1, lowByte( (int) nBytes)); delay(5);

  //store which optional data was collected
  EEPROM.write(2,Data2Record[EncoderIndex]); delay(5);
  EEPROM.write(3,Data2Record[D10Index]); delay(5);
  EEPROM.write(4,Data2Record[D14Index]); delay(5);
  EEPROM.write(5,Data2Record[D15Index]); delay(5);
  EEPROM.write(6,Data2Record[A0Index]); delay(5);
  EEPROM.write(7,Data2Record[A1Index]); delay(5);
  
  
  // Give 10 second delay until end. Allow battery to return back to normal state
  ledFlash(10,500,500); 
  
  digitalWrite(yellowLedPin,HIGH); // Turn yellow LED on.

}
//******************************************************************************************

#endif