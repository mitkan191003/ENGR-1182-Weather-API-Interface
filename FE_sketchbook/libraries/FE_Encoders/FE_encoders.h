//FE Controller Encoder Library
//to be included when using pins 2 and 3 with reflectance sensors as wheel encoders

#ifndef FE_ENCODERS_H
#define FE_ENCODERS_H

// ---------------------------------------------------------------------------
// Pin definitions
// ---------------------------------------------------------------------------

const byte encoderPinA   =  2; // pin 2 (interrupt 0) used for wheel counts 
const byte encoderPinB   =  3; // pin 3 (interrupt 1) used for wheel counts

// Quadrature Encoder
bool encoderSetA                  	 = false;     // Boolean quadrature encoder for sensor A
bool encoderSetB                  	 = false;     // Boolean quadrature encoder for sensor A
volatile int encoderPos              =     0;     // Position
volatile unsigned int encoderTotal   =     0;     // Total accumulation of marks
volatile uint16_t encoderForSum      =     0;     // Forward sum
volatile uint16_t encoderBackSum     =     0;     // Backward sum
volatile byte dir                    =     2;     // Direction: 1 = Forward; 0 = Reverse, 2 = No Direction.


//**************************************************************************
// Quadrature Encoders
//**************************************************************************

// (8) Interrupt on A changing state
void quadEncoderA()
{
  // Test transition
  encoderSetA = (digitalRead(encoderPinA) == HIGH);
  
  // Adjust counter + if A leads B
  // If encoderSetA is not equal to encoderSetB, add 1, else subtract 1
  encoderPos += (encoderSetA != encoderSetB) ? + 1 : -1;
  
  // Cumulative sum
  // Add one to current sum
  encoderTotal += 1;
  
  // Forward sum
  // If encoderSetA is not equal to encoderSetB, add 1, else add 0
  encoderForSum += (encoderSetA != encoderSetB) ? + 1 : 0;
  
  // Backward sum
  // If encoderSetA is not equal to encoderSetB, add 0, else subtract 1
  encoderBackSum += (encoderSetA != encoderSetB) ? 0 : +1;
  
  // Adjust direction
  // If encoderSetA is not equal to encoderSetB, 1 (forward), else 0 (reverse)
  dir = (encoderSetA != encoderSetB) ? 1 : 0;
}

// (06) Interrupt on B changing state
void quadEncoderB()
{
  // Test transition
  encoderSetB = (digitalRead(encoderPinB) == HIGH);
  
  // Adjust counter + if B follows A
  // If encoderSetA is equal to encoderSetB, add 1, else subtract 1
  encoderPos += (encoderSetA == encoderSetB) ? +1 : -1;
  
  // Cumulative sum
  // Add one to current sum
  encoderTotal += 1;
  
  // Forward sum
  // If encoderSetA is equal to encoderSetB, add 1, else add 0
  encoderForSum += (encoderSetA == encoderSetB) ? + 1 : 0;
  
  // Backward sum
  // If encoderSetA is equal to encoderSetB, add 0, else subtract 1
  encoderBackSum += (encoderSetA == encoderSetB) ? 0 : +1;
  
  // Adjust direction
  // If encoderSetA is equal to encoderSetB, 1 (forward), else 0 (reverse)
  dir = (encoderSetA == encoderSetB) ? 1 : 0;

}

void encoderSetup() 
{
    pinMode(encoderPinA,INPUT_PULLUP);      // Set encoderPinA mode
	pinMode(encoderPinB,INPUT_PULLUP);      // Set encoderPinB mode
	attachInterrupt(1,quadEncoderA,CHANGE); // Attach interrupt to A (pin 3 == 1)
	attachInterrupt(0,quadEncoderB,CHANGE); // Attach interrupt to A (pin 2 == 0)
	
    // Initialize quadrature encoder variables to 0
    encoderPos     = 0;
    encoderTotal   = 0;
    encoderForSum  = 0;
    encoderBackSum = 0;
}

//**************************************************************************
// Student Functions
//**************************************************************************

// Get Encoder Direction
int getEncoderDirection()
{
  //------------------------------------------------------------------------
  // getEncoderDirection();
  //
  // Returns a flag specifying the direction the vehicle is traveling. 
  // 1 = forward, 0 = backward.
  //
  // Example Call on Routine: 
  // int direction = getEncoderDirection();
  //
  // Input: none
  //
  // Output: none
  //-------------------------------------------------------------------------

 // recordData(); // Record Data.
  
  // Return global variable used in quadrature encoder
  return dir;
}

// Get Total Counts Detected
 int getTotalCounts()
{
  //------------------------------------------------------------------------
  // getTotalCounts();
  //
  // Returns total marks AEV has traveled.
  //
  // Example Call on Routine: 
  // int totalMarks = getTotalCounts();
  //
  // Input: none
  //
  // Output: none
  //-------------------------------------------------------------------------
  
  // recordData(); // Record Data.
  
  // Return global variable used in quadrature encoder
  return encoderTotal;
}


// (02) Get Total Backward Marks Traveled
int getBackwardCounts()
{
  //------------------------------------------------------------------------
  // getBackwardCounts();
  //
  // Returns total number of marks AEV has traveled in the reverse direction.
  //
  // Example Call on Routine: 
  // int backwardTotal = getBackwardCounts();
  //
  // Input: none
  //
  // Output: none
  //-------------------------------------------------------------------------

  //recordData(); // Record Data.
  
  // Return global variable used in quadrature encoder
  return abs(encoderBackSum);
}


// (03) Get Total Forward Marks Traveled
int getForwardCounts()
{
  //------------------------------------------------------------------------
  // getForwardCounts();
  //
  // Returns total number of marks AEV has traveled in the forward direction.
  //
  // Example Call on Routine: 
  // int forwardTotal = getForwardCounts();
  //
  // Input: none
  //
  // Output: none
  //-------------------------------------------------------------------------

 // recordData(); // Record Data.
  
  // Return global variable used in quadrature encoder
  return encoderForSum;
}


// Reflectance Sensor test
void reflectanceSensorTest()
{
  //------------------------------------------------------------------------
  // reflectanceSensorTest();
  //
  // Tests wheel count sensors
  //
  //
  // Input: none
  //
  // Output: none
  //------------------------------------------------------------------------- 
  
  int priordir                         = 0;         // Prior Direction.
  int priortotal                       = 0;         // Prior Total Counts.
  int priorpos                         = 0;         // Prior Positions.
  // Enter while loop and print encorder variables to serial monitor
  do
  {
    while(dir == 2) // Wait until we start spinning the wheel
    {
    }; 
    if(priordir != dir || priorpos != encoderPos || priortotal != encoderTotal) {
      // Print total counts
      Serial.print("Total Count: "); Serial.print(encoderTotal);
      
      // Print Forward Counts
      Serial.print("; Forward Counts: "); Serial.print(encoderForSum);
      
      // Print Backward Counts
      Serial.print("; Backward Counts: "); Serial.print(abs(encoderBackSum));
      
      // Print direction
      if(dir == 1) Serial.println("; Direction: Forward");
      if(dir == 0) Serial.println("; Direction: Reverse");
      
      // Put a delay in to avoid freeze
      delay(50);
      priordir = dir;
      priorpos = encoderPos;
      priortotal = encoderTotal;
    }
    
  }
  while(1);
}
//**************************************************************************

#endif