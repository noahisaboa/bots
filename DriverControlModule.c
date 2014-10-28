//////////////////////////////////////////////////////////////////////////////////////
//								Driver Control Module								//
//////////////////////////////////////////////////////////////////////////////////////
// This module allows a wide variety of configurations per driver. It allows drivers//
// to swap position at the change of the DRV_CURRENT_DRIVER variable to a defined   //
// enum value. New drivers can easily be added and have an effect throughout the    //
// rest of the programming. It also cleans controller values to be used later by    //
// other systems in the code, such as button down "events", or trimming joystick    //
// values.																			//
//////////////////////////////////////////////////////////////////////////////////////

// Set the DRV_CURRENT_DRIVER variable to whoever is preconfigured.
// Alternatively, you can also add your own settings.
// Add a name to DRV_Driver enum, then add configurations in DRV_setupConfig() using an if statement.
enum DRV_Driver {Default, Parker, Justin, Aaron};
// If you are unsure, leave the setting on Default. It is configured to be efficient and intuitive.
const DRV_Driver DRV_CURRENT_DRIVER = Default;

// Enum values to bind program functions with buttons.
enum DRV_RemoteFunction {LiftUp = 0, LiftDown, LiftJoy, LiftTopMacro, LiftBottomMacro, MecanumRightNormal, MecanumRightStrafe,
	MecanumLeftNormal, MecanumLeftStrafe, MecanumRotate,  ClawLiftUp, ClawLiftDown, ClawLiftJoy, ClawLiftTopMacro,
	ClawLiftBottomMacro, ClawWristUp, ClawWristDown, ClawWristJoy, ClawClamp, PidOverride, UNASSIGNED = 99};

// These arrays are available for other parts of the code to retrieve cleaned values.
short DRV_config[32]; // This array returns the bound button/joystick value from the controller.
bool DRV_controllerButtonsDown[32]; // This array returns only BUTTONS that have been pushed down ONCE. Comparable to onDown() event.

const int DRV_JOYSTICK_THRESHOLD = 20; // The trim for the joystick values.
const int DRV_INTERVALS_PER_SECOND = 50; // Hertz rate to check buttons.

void DRV_setupConfig() {
	writeDebugStreamLine("[Config]: The designated driver index is %i", DRV_CURRENT_DRIVER);
	//// DEFAULT ////
	// Setup default button binds, then let DRV_CURRENT_DRIVER override. No hollow values that way.
	// Joystick slots should only be channels or UNASSIGNED.
	// Mecanum controls should never be buttons.
	DRV_config[LiftUp] = Btn6U; // Button to move the main lift upward.
	DRV_config[LiftDown] = Btn6D; // Button to move the main lift downward.
	DRV_config[LiftJoy] = UNASSIGNED; // Joystick channel to manipulate the main lift.
	DRV_config[LiftTopMacro] = Btn8U; // Button to set the PID target of the main lift to the top.
	DRV_config[LiftBottomMacro] = Btn8D; // Button to set the PID target of the main lift to the bottom.
	DRV_config[MecanumRightNormal] = Ch3; // Joystick channel that controls right side wheels forward and backward movement.
	DRV_config[MecanumRightStrafe] = Ch4; // Joystick channel that controls the right side wheels strafing movement.
	DRV_config[MecanumLeftNormal] = Ch3; // Joystick channel that controls the left side wheels forward and backward movement.
	DRV_config[MecanumLeftStrafe] = Ch4; // Joystick channel that controls the left side wheels strafing movement.
	DRV_config[MecanumRotate] = Ch1; // Joystick channel that controls the rotating ability of both sides.
	DRV_config[ClawLiftUp] = Btn5U; // Button to move the claw lift upward.
	DRV_config[ClawLiftDown] = Btn5D; // Button to move the claw lift downward.
	DRV_config[ClawLiftJoy] = UNASSIGNED; // Joystick channel to manipulate the claw lift.
	DRV_config[ClawLiftTopMacro] = Btn7U; // Button to set the PID target of the claw lift to the top.
	DRV_config[ClawLiftBottomMacro] = Btn7D; // Button to set the PID target of the claw lift to the bottom.
	DRV_config[ClawWristUp] = UNASSIGNED; // Button to angle the claw wrist upward.
	DRV_config[ClawWristDown] = UNASSIGNED; // Button to angle the claw wrist downward.
	DRV_config[ClawWristJoy] = Ch2; // Joystick channel to manipulate the claw wrist's angle.
	DRV_config[ClawClamp] = Btn8R; // Button to toggle the claw's clamping.
	DRV_config[PidOverride] = Btn7L; // Button to toggle the override of a PID system. Must be used in combination with a PID system button.

	if (DRV_CURRENT_DRIVER == Parker) {
		//// PARKER ////
		DRV_config[LiftUp] = UNASSIGNED;
		DRV_config[LiftDown] = UNASSIGNED;
		DRV_config[LiftJoy] = Ch2;
		DRV_config[LiftTopMacro] = Btn8U;
		DRV_config[LiftBottomMacro] = Btn8D;
		DRV_config[MecanumRightNormal] = Ch3;
		DRV_config[MecanumRightStrafe] = Ch4;
		DRV_config[MecanumLeftNormal] = Ch3;
		DRV_config[MecanumLeftStrafe] = Ch4;
		DRV_config[MecanumRotate] = Ch1;
		DRV_config[PidOverride] = Btn7U;
	} else if (DRV_CURRENT_DRIVER == Justin) {
		//// JUSTIN ////
		DRV_config[LiftUp] = Btn6U;
		DRV_config[LiftDown] = Btn6D;
		DRV_config[LiftJoy] = UNASSIGNED;
		DRV_config[MecanumRightNormal] = Ch2;
		DRV_config[MecanumRightStrafe] = Ch1;
		DRV_config[MecanumLeftNormal] = Ch3;
		DRV_config[MecanumLeftStrafe] = Ch4;
		DRV_config[MecanumRotate] = UNASSIGNED;
		DRV_config[PidOverride] = Btn7U;
	} else if (DRV_CURRENT_DRIVER == Aaron) {
		//// AARON ////
		DRV_config[LiftUp] = UNASSIGNED;
		DRV_config[LiftDown] = UNASSIGNED;
		DRV_config[LiftJoy] = Ch2;
		DRV_config[LiftTopMacro] = Btn5U;
		DRV_config[LiftBottomMacro] = Btn5D;
		DRV_config[MecanumRightNormal] = Ch3;
		DRV_config[MecanumRightStrafe] = Ch4;
		DRV_config[MecanumLeftNormal] = Ch3;
		DRV_config[MecanumLeftStrafe] = Ch4;
		DRV_config[MecanumRotate] = Ch1;
		DRV_config[PidOverride] = Btn7U;
	}
}

// Trims the joystick channel's value. This eliminates joystick noise, and motor whining (Yay!).
int DRV_trimChannel(DRV_RemoteFunction channel, int trim = DRV_JOYSTICK_THRESHOLD) {
	int value = vexRT[DRV_config[channel]];
	if (abs(value) <= trim) {
		return 0;
	}
	return value;
}

// The loop that handles button down "events".
task DRV_buttonHandler()
{
	// Setup the drivers.
	DRV_setupConfig();
	// Clear the buttons.
	for (int i = 0; i < sizeof(DRV_controllerButtonsDown) / 2; i++) {
		DRV_controllerButtonsDown[i] = false;
	}
	bool lastController[32] = DRV_controllerButtonsDown;
	// Set all button presses to true if the button is down.
	// After button value is checked, must be cancelled! Use DRV_controllerButtonsDown[button] = false
	while(true) {
		for (int button = 0; button < sizeof(DRV_config) / 2; button++) {
			if (vexRT[DRV_config[button]] == true) {
				if (lastController[button] != true) {
					// New button down. Add it to the Controller.
					writeDebugStreamLine("[Control]: Button %i down.", button);
					DRV_controllerButtonsDown[button] = true;
					lastController[button] = true;
				}
			} else {
				lastController[button] = false;
			}
		}
		// Wait at a set interval, to allow processing power to other modules.
		wait1Msec(1000 / DRV_INTERVALS_PER_SECOND);
	}
}
