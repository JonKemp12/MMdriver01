// Do not remove the include below
#include "MMvector.h"
#include "HardwareSerial.h"

/*
 * This driver will read vector commands from the Serial input
 * and drive the MiniMover using the provided steps using
 * synchronous point to point using time as the basis.
 * Commands are byte array of the form:
 * 'V',sp,m1,m2,m3,m4,m5,m6
 * where sp is speed (byte) - 1-16
 * m1 to m6 are motor vectors (2 bytes) - -32,768 to +32,767
 */
// Command line handling variables:
// CmdStatus values
const byte CmdDone = 0;
const byte CmdReady = 1;
const byte CmdErr = -1;
// CmdBuff control
byte	CmdStatus;				// Holds the CmdBuf status for hand-shaking
byte	CmdBuff[16];			// Bytes read in by serial event
byte	CmdBuffLen = 0;			// Current number of bytes in buffer
byte	CmdBuffEnd = 0;			// Required number of bytes for this command.

// Global variables
byte 	speed 		= 0;				// Speed multiplier 1 to 16
int		vector[]	= {0,0,0,0,0,0};	// Store of given signed vectors

static int driving;					// Number of motors still moving
static int useJoystick = 0;			// Command to enable the joyStick control


//The setup function is called once at startup of the sketch
void setup()
{
	// initialise serial:
	Serial.begin(9600);
	// Setup IO
	for (int thisPin = 0; thisPin < 4; thisPin++) {
		pinMode(pattPin[thisPin], OUTPUT);
	}
	for (int thisPin = 0; thisPin < 3; thisPin++) {
		pinMode(motAddr[thisPin], OUTPUT);
	}

	pinMode(strobe, OUTPUT);		// Strobe pin is an output
	digitalWrite(strobe, HIGH);

	pinMode(GripClosed, INPUT);		// Grip switch - high when closed.
	pinMode(Panic, INPUT);			// Panic switch - high when OK.

	pinMode(Trigger, INPUT);
	pinMode(LeftBut, INPUT);
}

/*
 * loop() is called endlessly.
 * It checks for a command (CmdStatus = CmdReady) -
 *  if so, process the command setting up drive vectors etc.
 * It then checks if there are motors to drive and the time delay has expired -
 *  if so calculates which motors need stepping and output the patterns.
 */

void loop()
{
	if (CmdStatus == CmdReady)	{	// Process new command
		switch(CmdBuff[0]) {
		case 'V':		// Vector command
			// Parse buffer
			// Calc waitTime is #ms = 1 to 16 x minDelay
			speed = 16 - (unsigned int)(CmdBuff[1] & 0x000f);		// Get speed value low 4bits 1 to 16

			// Turn bytes into unsigned integers & direction
			vector[0] = ((CmdBuff[2]<<8) | CmdBuff[3]);
			vector[1] = ((CmdBuff[4]<<8) | CmdBuff[5]);
			vector[2] = ((CmdBuff[6]<<8) | CmdBuff[7]);
			vector[3] = ((CmdBuff[8]<<8) | CmdBuff[9]);
			vector[4] = ((CmdBuff[10]<<8) | CmdBuff[11]);
			vector[5] = ((CmdBuff[12]<<8) | CmdBuff[13]);

			/* IFDEBUG */
			Serial.print(" Got cmd:");
			Serial.print(CmdBuff[0]);
			Serial.print(", ");
			Serial.print(speed);
			Serial.print(", ");
			Serial.print(vector[0]);
			Serial.print(", ");
			Serial.print(vector[1]);
			Serial.print(", ");
			Serial.print(vector[2]);
			Serial.print(", ");
			Serial.print(vector[3]);
			Serial.print(", ");
			Serial.print(vector[4]);
			Serial.print(", ");
			Serial.print(vector[5]);
			Serial.println();

			CmdBuffLen = 0;
			CmdBuffEnd = 0;
			CmdStatus = CmdDone;		// Allow a new command now.

			// Now we have the the data assembled initialise the move:
			driving = initDriver();

			/* IF DEBUG
			 *
			Serial.println("Motor:\tvec:\tnum:\tdelay: ");
			for (int i = 0; i < NMOTORS; ++i) {
				Serial.print(i);
				Serial.print("\t");
				Serial.print(vector[i]);
				Serial.print("\t");
				Serial.print(numSteps[i]);
				Serial.print("\t");
				Serial.print(stepDelay[i]);
				Serial.println("");
			}
			 */
			break;

		case 'J':
			useJoystick = (unsigned int)(CmdBuff[1]);
			CmdBuffLen = 0;
			CmdBuffEnd = 0;
			CmdStatus = CmdDone;		// Allow a new command now.
			break;

		case 'S':
			reportStatus();
			CmdBuffLen = 0;
			CmdBuffEnd = 0;
			CmdStatus = CmdDone;		// Allow a new command now.
			break;

		default:	// Unknown command so report error.
			CmdStatus = CmdErr;
			break;
		}
	}

	if (CmdStatus == CmdErr) {
		Serial.print(" Err: [");
		Serial.print(CmdBuff[0]);
		Serial.println("]");
		CmdBuffLen = 0;
		CmdBuffEnd = 0;
		CmdStatus = CmdDone;			// Reset
	}

	// If there are motors to drive
	if (driving > 0) {
		driving = doDrive();
	}

	// If Joystick control is enabled
	if (useJoystick != 0) {
		driving = jsDrive();
	}

}

/* SerialEvent reads bytes from the output stream.
 * It looks checks the first byte to be a 'known' command which determines how
 * many bytes follow. When all bytes are read, it sets flag CmdStatus to CmdReady
 * and will not read further bytes until CmdStatus is set to CmdDone.
 * If the first byte is not recognised it sets CmdStatus to CmdError and discards
 * any received bytes.
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
	if (Serial.available()) {
		// Action depends on status
		switch(CmdStatus) {
		// Build command buffer
		case CmdDone:
			CmdBuff[CmdBuffLen++] = (byte)Serial.read();
			if (CmdBuffLen == 1) {	// This is command designator.
				switch(CmdBuff[0]) {
				case 'V':			// Vector command "'V',sp,m1,m2,m3,m4,m5,m6"
					CmdBuffEnd = 14;
					break;

				case 'J':			// Enable/disable joystick control "'J',n"
					CmdBuffEnd = 2;
					break;

				case 'S':			// Report status "'S'"
					CmdBuffEnd = 1;
					break;

				default:
					CmdStatus = CmdErr;		// Else report the error
					break;
				}
			}
			if (CmdBuffLen >= CmdBuffEnd)	// Got the full complement
				CmdStatus = CmdReady;
			break;

			// loop is working on the command.
		case CmdReady:
			break;

			// Found error so discard
		default:
		case CmdErr:
			Serial.read();
			break;
		}
	}
}

extern int	motPos[];	// Accumulated motor position
extern int	numSteps[];	// Number of steps to go
extern int	dir[];
/*
 * reportStatus() :
 *   print out status of driver in human readable form.
 */
void
reportStatus() {
	// First report absolute values:
	Serial.print("A ");
	Serial.print(speed);
	for (int i = 0; i < NMOTORS; i++) {
		Serial.print("," );
		Serial.print(motPos[i]);
	}

	// Driving and numSteps to go:
	Serial.print(" | D ");
	Serial.print(driving);
	for (int i = 0; i < NMOTORS; i++) {
		Serial.print("," );
		Serial.print(numSteps[i] * dir[i]);
	}

	  // Joystick status:
	Serial.print(" | J ");
	Serial.print(useJoystick);

	  Serial.print(": L/R:" );
	  Serial.print(analogRead(LeftRight));
	  Serial.print(", F/B:" );
	  Serial.print(analogRead(ForwardBack));
	  Serial.print(", Hat:" );
	  Serial.print(map(analogRead(Hat), 350, 950, 0, 5));
	  Serial.print(", Tr:" );
	  Serial.print(digitalRead(Trigger));
	  Serial.print(", Lt:" );
	  Serial.print(digitalRead(LeftBut));
	  Serial.print(", Rt:" );
	  Serial.print(analogRead(RightBut));
	  Serial.print(", Bot:" );
	  Serial.print(analogRead(BottomBut));
	  Serial.println("" );

}
