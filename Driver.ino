/*
 * Driver.ino
 *
 * This file contains all the code to initialise
 * and driver the motors.
 *
 * It picks up the vector array & speed and calculates the steps and delays.
 *
 *
 *  Created on: 16 May 2012
 *      Author: jon
 */

#include "MMvector.h"

/* Motion control variables:
 * Synchronous point to point is achieved by calculating the time
 * taken each motor to complete it's number of steps at it's max step rate.
 * The longest time is multiplied by the required speed command and the result is
 * then used to scale all the others to result in the same "elapsed" time thus
 * all motors should start and finish at the same time.
 *
 * Timing is calculated in 'ticks' - currently 0.1 ms (== 100 us).
 */

// Global externals
extern byte speed;
extern int	vector[];

// Constant arrays for motor characteristics:
// minDelay = max speed of each motor. 350 steps/s = 28 ticks
// max torque is about 50 steps/s, max around 350

//	const byte minDelay[] = {55, 40, 40, 32, 32, 28};
byte 			minDelay[] = {24, 30, 40, 32, 32, 16};//test values - to be determined by testing!
unsigned long 	maxTime;						// Calculated longest time for any motor.
unsigned int 	numSteps[]  = {0,0,0,0,0,0};	// Absolute values of steps for each motor
int 			dir[] 		= {0,0,0,0,0,0};	// The direction -1 or +1
unsigned int 	stepDelay[] = {0,0,0,0,0,0};	// Calculated delay between steps (velocity)

byte 			rampSteps[] = {0,0,0,0,0,0};	// Number of steps to ramp Down
unsigned int 	rampDelay[] = {0,0,0,0,0,0};	// Current rampDelay

int 			curDelay[]  = {0,0,0,0,0,0};	// Current, varying delay between steps

int				motPos[] 	= {0,0,0,0,0,0};	// Accumulated motor position

unsigned long	debugTime	= 0;				// Used for debug output.

// Motor Position control
// max and min absolute limits (end stops from home position)
const int maxPos[] = {9999, 9999, 9999, 9999, 9999, 9999};
const int minPos[] = {-9999, -9999, -9999, -9999, -9999, -9999};

// Timing control
unsigned long 		lastTime = 0;		// time, in ticks, last time through loop
unsigned long		timeNow  = 0;		// current time in ticks.
unsigned int		timeInc = 0;		// Elapsed time since last time

const byte motPatt[]  = { 				// Array of 8 stepper motor patterns.
		B0101,
		B0100,
		B0110,
		B0010,
		B1010,
		B1000,
		B1001,
		B0001 };

int
initDriver() {
	int activeMotors = 0;

	maxTime = 0;
	for (int i = 0; i < NMOTORS; i++) {
		// Get abs for vector and direction (makes calcs easier!)
		if (vector[i] < 0) {
			dir[i] = -1;
			numSteps[i] = abs(vector[i]);
		} else {
			dir[i] = 1;
			numSteps[i] = vector[i];
		}

		// Find maxTime for the move
		if (numSteps[i] * minDelay[i] > maxTime) {
			maxTime = numSteps[i] * minDelay[i];
		}

		// Set rampSteps
		if (numSteps[i] < NUMRAMPSTEPS * 2) {
			rampSteps[i] = numSteps[i] / 2;
		} else {
			rampSteps[i] = NUMRAMPSTEPS;
		}
	}

	// Now calculate to actual delay between steps:
	for (int i = 0; i < NMOTORS; i++) {
		if (numSteps[i] > 0 ) {
			activeMotors++;

			// Multiply up each delay so all motors run for the same elaped time.
			stepDelay[i] = (int) ((maxTime * speed) / numSteps[i]);
			// and calculate initial delay values including ramp
			// rampDelay[i] = rampSteps[i] * RAMPRATE;
			rampDelay[i] = MAXRAMPDELAY;
			curDelay[i]  = stepDelay[i] + rampDelay[i];
		}
	}

	// Initialise the loop
	lastTime = micros() / 100;
	timeInc = 0;

	return activeMotors;
}

int
doDrive() {
	// Find elapsed time in ticks
	timeNow = micros() / 100;
	if (timeNow < lastTime) {	// Rolled over!
		timeInc = (unsigned int)(lastTime ^ -1) + timeNow + 1;
	} else {
		timeInc = (unsigned int)(timeNow - lastTime);
	}

	lastTime = timeNow;

	debugTime += timeInc;

	// Here to step motors
	int activeMotors = 0;		// incremented if any motor needs to move still.
	boolean debugPrint = false;	// Print only if changed!

	// For each motor vector
	for (int mot = 0; mot < NMOTORS; mot++) {
		if (numSteps[mot] == 0)		// Nothing for this one
			continue;

		activeMotors++;			// Increment number of motors still moving

		if ((curDelay[mot] -= timeInc) < 0) {	// Delay has expired so move it!
			// Check if closing grip and grip closed.
			if (mot == GRIPPER && dir[mot] < 0 && (digitalRead(GripClosed) == 0) ) {
				numSteps[mot] = 0;
				break;				// Stop checking this one
			}
			// Check motor max limit
			if (dir[mot] > 0 && motPos[mot] > maxPos[mot]) {
				numSteps[mot] = 0;
				break;				// Stop checking this one
			}
			// Check motor min limit
			if (dir[mot] < 0 && motPos[mot] < minPos[mot]) {
				numSteps[mot] = 0;
				break;				// Stop checking this one
			}

			debugPrint = true;

			// Step the motor:
			numSteps[mot]--;
			motPos[mot] += dir[mot];

			// Find motor pattern and output;
			int pIndex = motPos[mot] & 7;
			byte mPat = motPatt[pIndex];
			digitalWrite(pattPin[0], bitRead(mPat, 0));
			digitalWrite(pattPin[1], bitRead(mPat, 1));
			digitalWrite(pattPin[2], bitRead(mPat, 2));
			digitalWrite(pattPin[3], bitRead(mPat, 3));

			//Output motor addr
			digitalWrite(motAddr[0], bitRead(mot, 0));
			digitalWrite(motAddr[1], bitRead(mot, 1));
			digitalWrite(motAddr[2], bitRead(mot, 2));

			// Strobe the data through:
			digitalWrite(strobe, LOW);
			digitalWrite(strobe, HIGH);

			// Now recalc delay until next time
			// Calc new delay
			curDelay[mot] += stepDelay[mot];	// Add back non-ramp delay
			// This accounts for over-shoot too!
			// Add ramp down if getting to end!
			if (numSteps[mot] < rampSteps[mot]) {
				rampDelay[mot] += RAMPRATE;		// Increase rampDelay
				curDelay[mot] += rampDelay[mot];
			} else if (rampDelay[mot] > 0) {	// Add ramp up delay using until it is zero
				rampDelay[mot] -= RAMPRATE;		// Reduce the rampDelay;
				curDelay[mot] += rampDelay[mot];
			}
		}

		/* IF DEBUG
		if (debugPrint) {
			Serial.print(" ");
			Serial.print(debugTime);
			for (int i = 0; i < NMOTORS; i++) {
				Serial.print(", ");
				// Serial.print(motPos[i]);
				Serial.print(motPos[i]);
			}
			Serial.println("");
		}
		 */
	} // end of foreach motor
	return activeMotors;
}

