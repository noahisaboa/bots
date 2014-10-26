// List of driver settings. Buttons are bound differently per driver.
// Set the CURRENT_DRIVER variable to whoever is preconfigured.
enum Driver {Default, Parker, Justin, Aaron};
const Driver CURRENT_DRIVER = Default;

enum RT_FUNCTION {LiftUp = 0, LiftDown, LiftJoy, LiftTopMacro, LiftBottomMacro, MecanumRightNormal, MecanumRightStrafe,
	MecanumLeftNormal, MecanumLeftStrafe, MecanumRotate,  ClawLiftUp, ClawLiftDown, ClawLiftJoy, ClawLiftTopMacro,
	ClawLiftBottomMacro, ClawWristUp, ClawWristDown, ClawWristJoy, ClawClamp, PidOverride, UNASSIGNED = 99};

short DRIVER_CONFIG[32];
bool ControllerButtonsDown[32];

const int DRV_JOYSTICK_THRESHOLD = 20; // The trim for the joystick values.
const int DRV_INTERVALS_PER_SECOND = 50; // Hertz rate to check buttons.

void setupConfig() {
	writeDebugStreamLine("[Config]: The designated driver index is %i", CURRENT_DRIVER);
	//// DEFAULT ////
	// Setup default button binds, then let CURRENT_DRIVER override.
	DRIVER_CONFIG[LiftUp] = Btn6U;
	DRIVER_CONFIG[LiftDown] = Btn6D;
	DRIVER_CONFIG[LiftJoy] = UNASSIGNED; // Joystick slots should only be channels or UNASSIGNED.
	DRIVER_CONFIG[LiftTopMacro] = Btn8U;
	DRIVER_CONFIG[LiftBottomMacro] = Btn8D;
	DRIVER_CONFIG[MecanumRightNormal] = Ch3; // Mecanum controls should never be buttons.
	DRIVER_CONFIG[MecanumRightStrafe] = Ch4;
	DRIVER_CONFIG[MecanumLeftNormal] = Ch3;
	DRIVER_CONFIG[MecanumLeftStrafe] = Ch4;
	DRIVER_CONFIG[MecanumRotate] = Ch1;
	DRIVER_CONFIG[ClawLiftUp] = Btn5U;
	DRIVER_CONFIG[ClawLiftDown] = Btn5D;
	DRIVER_CONFIG[ClawLiftJoy] = UNASSIGNED;
	DRIVER_CONFIG[ClawLiftTopMacro] = Btn7U;
	DRIVER_CONFIG[ClawLiftBottomMacro] = Btn7D;
	DRIVER_CONFIG[ClawWristUp] = UNASSIGNED;
	DRIVER_CONFIG[ClawWristDown] = UNASSIGNED;
	DRIVER_CONFIG[ClawWristJoy] = Ch2;
	DRIVER_CONFIG[ClawClamp] = Btn8R;
	DRIVER_CONFIG[PidOverride] = Btn7L;

	if (CURRENT_DRIVER == Parker) {
		//// PARKER ////
		DRIVER_CONFIG[LiftUp] = UNASSIGNED;
		DRIVER_CONFIG[LiftDown] = UNASSIGNED;
		DRIVER_CONFIG[LiftJoy] = Ch2;
		DRIVER_CONFIG[LiftTopMacro] = Btn8U;
		DRIVER_CONFIG[LiftBottomMacro] = Btn8D;
		DRIVER_CONFIG[MecanumRightNormal] = Ch3;
		DRIVER_CONFIG[MecanumRightStrafe] = Ch4;
		DRIVER_CONFIG[MecanumLeftNormal] = Ch3;
		DRIVER_CONFIG[MecanumLeftStrafe] = Ch4;
		DRIVER_CONFIG[MecanumRotate] = Ch1;
		DRIVER_CONFIG[PidOverride] = Btn7U;
	} else if (CURRENT_DRIVER == Justin) {
		//// JUSTIN ////
		DRIVER_CONFIG[LiftUp] = Btn6U;
		DRIVER_CONFIG[LiftDown] = Btn6D;
		DRIVER_CONFIG[LiftJoy] = UNASSIGNED;
		DRIVER_CONFIG[MecanumRightNormal] = Ch2;
		DRIVER_CONFIG[MecanumRightStrafe] = Ch1;
		DRIVER_CONFIG[MecanumLeftNormal] = Ch3;
		DRIVER_CONFIG[MecanumLeftStrafe] = Ch4;
		DRIVER_CONFIG[MecanumRotate] = UNASSIGNED;
		DRIVER_CONFIG[PidOverride] = Btn7U;
	} else if (CURRENT_DRIVER == Aaron) {
		//// AARON ////
		DRIVER_CONFIG[LiftUp] = UNASSIGNED;
		DRIVER_CONFIG[LiftDown] = UNASSIGNED;
		DRIVER_CONFIG[LiftJoy] = Ch2;
		DRIVER_CONFIG[LiftTopMacro] = Btn5U;
		DRIVER_CONFIG[LiftBottomMacro] = Btn5D;
		DRIVER_CONFIG[MecanumRightNormal] = Ch3;
		DRIVER_CONFIG[MecanumRightStrafe] = Ch4;
		DRIVER_CONFIG[MecanumLeftNormal] = Ch3;
		DRIVER_CONFIG[MecanumLeftStrafe] = Ch4;
		DRIVER_CONFIG[MecanumRotate] = Ch1;
		DRIVER_CONFIG[PidOverride] = Btn7U;
	}
}

int trimChannel(int value, int trim = DRV_JOYSTICK_THRESHOLD) {
	if (abs(value) <= trim) {
		return 0;
	}
	return value;
}

task buttonHandler()
{
	// Setup the drivers.
	setupConfig();
	// Clear the buttons.
	for (int i = 0; i < sizeof(ControllerButtonsDown) / 2; i++) {
		ControllerButtonsDown[i] = false;
	}
	bool lastController[32] = ControllerButtonsDown;
	// Set all button presses to true if the button is down.
	// After button value is checked, must be cancelled! Use ControllerButtonsDown[button] = false
	while(true) {
		for (int button = 0; button < sizeof(DRIVER_CONFIG) / 2; button++) {
			if (vexRT[DRIVER_CONFIG[button]] == true) {
				if (lastController[button] != true) {
					// New button down. Add it to the Controller.
					writeDebugStreamLine("[Control]: Button %i down!", button);
					ControllerButtonsDown[button] = true;
					lastController[button] = true;
				}
			} else {
				lastController[button] = false;
			}

		}
		wait1Msec(1000 / DRV_INTERVALS_PER_SECOND);
	}
}
