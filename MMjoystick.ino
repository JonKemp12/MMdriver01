/*
 * MMjoystick.ino
 *
 * Routines to read the joystick and set driver values.
 *
 *  Created on: 17 May 2012
 *      Author: jon
 */
#include "MMvector.h"

extern byte 		minDelay[];
extern unsigned int	numSteps[];
extern int			dir[];
extern unsigned int	stepDelay[];
extern unsigned int	rampDelay[];
extern byte			rampSteps[];
extern int			curDelay[];


long interval = 500;
long previousMillis = 0;

void
joystickTest() {
	unsigned long currentMillis = millis();

	  if(currentMillis - previousMillis > interval) {
	    // save the last time you blinked the LED
	    previousMillis = currentMillis;
		  // print the results to the serial monitor:
		  Serial.print("** L/R:" );
		  Serial.print(map(analogRead(LeftRight),0,1023, -3, 3));
		  Serial.print(", F/B:" );
		  Serial.print(map(analogRead(ForwardBack),0,1023, -3, 3));
		  Serial.print(", Hat:" );
		  Serial.print(map(analogRead(Hat), 350, 950, 0, 5));
		  Serial.print(", Tr:" );
		  Serial.print(digitalRead(Trigger));
		  Serial.print(", Lt:" );
		  Serial.print(digitalRead(LeftBut));
		  Serial.print(", Rt:" );
		  Serial.print(map(analogRead(RightBut), 0, 1023, 0, 1));
		  Serial.print(", Bot:" );
		  Serial.print(map(analogRead(BottomBut), 0, 1023, 0, 1));
		  Serial.println(" **" );
	  }
}

int
jsDrive() {
	int activeMotors = 0;

	int openGrip = analogRead(RightBut);
	int closeGrip = digitalRead(LeftBut);


	if (openGrip < 512) {	// Rt = open gripper
//		  Serial.print(", Rt:" );
//		  Serial.print(openGrip);

		if ( numSteps[GRIPPER] == 0) {	// Start moving
			dir[GRIPPER] = 1;
			numSteps[GRIPPER] = 2;
			rampSteps[GRIPPER] = 0;
			stepDelay[GRIPPER] = minDelay[GRIPPER] * 4;
			rampDelay[GRIPPER] = MAXRAMPDELAY;
			curDelay[GRIPPER] = 0;
		} else {
			if (dir[GRIPPER] == 1) { 				// Going this way
				if (numSteps[GRIPPER] < NUMRAMPSTEPS) {
					numSteps[GRIPPER]++;
					rampSteps[GRIPPER] = numSteps[GRIPPER]-1;		// Push ramp down point
				}
			} else {							// Going other way so
				numSteps[GRIPPER]--;			// Accelerate slow down
			}
		}
	} else if (closeGrip == 0) {	// Left t = close gripper
		//		  Serial.print(", Lt:" );
		//		  Serial.print(closeGrip);

		if ( numSteps[GRIPPER] == 0) {	// Start moving
			dir[GRIPPER] = -1;
			numSteps[GRIPPER] = 2;
			rampSteps[GRIPPER] = 0;
			stepDelay[GRIPPER] = minDelay[GRIPPER] * 4;
			rampDelay[GRIPPER] = MAXRAMPDELAY;
			curDelay[GRIPPER] = 0;
		} else {
			if (dir[GRIPPER] == -1) { 				// Going this way
				if (numSteps[GRIPPER] < NUMRAMPSTEPS) {
					numSteps[GRIPPER]++;
					rampSteps[GRIPPER] = numSteps[GRIPPER]-1;		// Push ramp down point
				}
			} else {							// Going other way so
				numSteps[GRIPPER]--;			// Accelerate slow down
			}
		}
	}

	// joystickTest();
	for (int i = 0; i < NMOTORS; i++) {
		if (numSteps[i] > 0) {
			activeMotors++;
		}
	}
	return activeMotors;
}
