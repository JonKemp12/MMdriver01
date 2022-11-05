// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef MMvector_H_
#define MMvector_H_
#include "Arduino.h"
//add your includes for the project MMvector here
/*
 *  * Acceleration algorithm:
 * The basis of this simple algorithm is to accelerate the fastest motor
 * from 50 steps/s to 350 steps/s over 20 steps linearly.
 * This will be done by simply adding an extra variable rampDelay to the
 * calculated 'stepDelay'. As the total of these extra ramp delays will be the same
 * for all motors they will all start and finish at the same time.
 * Which is maxTime + 2 x totalRampDelay.
 * rampDelay is 200 to 0 ticks in 21 steps = 10 ticks decrement each step.
 */
#define MAXRAMPDELAY	200		// The bottom of the ramp
#define NUMRAMPSTEPS	20		// The number of steps in the ramp
#define RAMPRATE 		10		// MAXRAMPDELAY / NUMRAMPSTEPS;

#define GRIPPER 5				// Address/index of gripper motor (and last motor)
#define	NMOTORS	6				// Number of motors

// Digital IO control
const byte pattPin[]  = {4, 5, 6, 7};	// Array of pin numbers for motor patterns
const byte motAddr[]  = {8, 9, 10};		// Array of pins for motor addr (1 to 6)
const byte strobe	  = 11;				// Data strobe pin
const byte GripClosed = 12;				// Gripped switch input
const byte Panic	  = 13;				// Panic stop switch input

// Joystick pin numbers:
const int Trigger =  2;      // Trigger switch input
const int LeftBut =  3;		// Left button input
const int LeftRight = A0;	// Left/right joystick
const int ForwardBack = A1;	// Forward / back joystick
const int Hat = A2;			// Hat input
const int RightBut = A3;	// Right button input
const int BottomBut = A4;	// Bottom button


// Called routines:
int		initDriver();
int		doDrive();
void	joystickTest();
void	reportStatus();
int		jsDrive();

//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project MMvector here




//Do not add code below this line
#endif /* MMvector_H_ */
