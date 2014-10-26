enum PID_Type {MainLift = 0, ClawLift, ClawWrist};

bool PID_ENABLED[] = {true, false, false}; // Non-constant allows for emergency override during run time.
const bool PID_INTEGRAL_ENABLED[] = {false, false, false};
const bool PID_DERIVATIVE_ENABLED[] = {false, false, false};
const int PID_POWER_MAX = 127;
const int PID_POWER_MIN = -127;
int PID_POWER_POSITIVE_DEADBAND[] = {32, 16, 8}; // Amount of motor play that allows minimal movement.
int PID_POWER_NEGATIVE_DEADBAND[] = {0, -4, -8};
const int PID_INTEGRAL_LIMIT[] = {50, 50, 50};
const int PID_INTERVALS_PER_SECOND = 100; // Essentially Hertz.

const float PID_SENSOR_SCALE[] = {1, 1, 1}; // If scaling of the encoder is necessary.
const float PID_INPUT_SCALE[] = {77.09, 77.09, 2.3}; // Set this to be {ENCODER TICKS / INCH, ENCODER TICKS / INCH, ENCODER TICKS / DEGREE}
float PID_target[] = {0, 0, 0}; // For setting the target height. Do not touch.
float PID_actual[] = {0, 0, 0}; // For detecting actual height. Do not touch.
float PID_SIGNAL_GENERATOR_MAX[] = {0.96, 0.97, 0.98}; // Percentage value for horse and carrot.
int PID_SYSTEM_HYSTERESIS[] = {8, 6, 4}; // Solves for motor jogging without breaking PID.
float PID_TARGET_MAX[] = {2000, 750, 1023}; // Max height from the origin.

const int PID_OVERLOAD_THRESHOLD[] = {450, 200, 50}; // If the PID is behind this much, activates overload LED.
short PID_blinkIdDisabled[] = {NULL, NULL, NULL}; // Placeholders for blink task ID's
short PID_blinkIdOverloaded[] = {NULL, NULL, NULL};

// PID Loop constants for Proportional, Integral, and Derivative.
const float PID_KP[] = {0.80, 0.80, 0.80};
const float PID_KI[] = {0.00, 0.00, 0.00};
const float PID_KD[] = {0.00, 0.00, 0.00};

float PID_CONTROL_INCREMENTER_UP[] = {0.625, 0.1, 0.1}; // How much controller button should add/subtract to the target.
float PID_CONTROL_INCREMENTER_DOWN[] = {0.825, 0.1, 0.1};

enum PID_Internal {filteredTarget, power, error, lastError, integral, derivative, lastTarget, lastActual};

float PID_internals[3][8];

void pidCycle(PID_Type index) {
	if (PID_ENABLED[index]) {
		// Grab the actual sensor value, then scale it (if necessary).
		PID_internals[index][lastActual] = PID_actual[index];
		if (index == MainLift) {
			PID_actual[index] = SensorValue[liftEncoder] * PID_SENSOR_SCALE[index];
		} else if (index == ClawLift) {
			PID_actual[index] = SensorValue[clawLiftEncoder] * PID_SENSOR_SCALE[index];
		} else if (index == ClawWrist) {
			PID_actual[index] = SensorValue[clawWristPot] * PID_SENSOR_SCALE[index];
		}

		// Target filter using a signal generator (horse and carrot method).
		PID_internals[index][filteredTarget] = PID_SIGNAL_GENERATOR_MAX[index] * PID_internals[index][lastTarget]
				+ (1.00 - PID_SIGNAL_GENERATOR_MAX[index]) * PID_target[index];
		PID_internals[index][lastTarget] = PID_internals[index][filteredTarget];

		if (PID_target[index] > PID_TARGET_MAX[index]) {
			PID_target[index] = PID_TARGET_MAX[index];
		} else if (PID_target[index] < 0) {
			PID_target[index] = 0;
		}

		// Calculate the error.
		PID_internals[index][error] = PID_actual[index] - PID_internals[index][filteredTarget];
		if (abs(PID_internals[index][error]) <= PID_SYSTEM_HYSTERESIS[index]) {
			PID_internals[index][error] = 0;
		}

		if (index == MainLift) {
			// Button is not yet installed! Will cause problems.
			/*if (PID_actual[index] == 0 && (SensorValue[liftTouch] != 1)) {
				// If we are supposedly at 0, and are not touching the bottom,
				// increase the error to meet the bottom.
				PID_internals[index][error] = -100;
			} else*/ if (PID_internals[index][error] < 0 && SensorValue[liftTouch] == 1) {
				// If we are at the bottom, and we are supposedly not,
				// reset the error, actual, and encoder.
				PID_internals[index][error] = 0;
				PID_actual[index] = 0;
				SensorValue[liftEncoder] = 0;
			}
		} else if (index == ClawLift) {
			// Not sure how we will do this yet.
		} else if (index == ClawWrist) {
			// Its a potentiometer, should (hopefully) handle itself.
		}

		if (PID_INTEGRAL_ENABLED[index]) {
			// Accumulate error to fill the gap.
			if (abs(PID_internals[index][error]) < PID_INTEGRAL_LIMIT[index]) {
				PID_internals[index][integral] += PID_internals[index][error];
			}
			if(PID_KI[index] == 0)
			    PID_internals[index][integral] = 0;
		} else {
			PID_internals[index][integral] = 0;
		}

		if (PID_DERIVATIVE_ENABLED[index]) {
			// Predict future change via derivative.
			PID_internals[index][derivative] = (PID_internals[index][error] - PID_internals[index][lastError]);
			PID_internals[index][lastError] = PID_internals[index][error];
		} else {
			PID_internals[index][derivative] = 0;
		}

		// Calculate the power necessary to correct the found error.
		// Negations are to keep error and power values in a logical context.
		PID_internals[index][power] = -1 * ((PID_KP[index] * PID_internals[index][error])
				+ (PID_KI[index] * PID_internals[index][integral])
				+ (PID_KD[index] * PID_internals[index][derivative]));

		// Limit the power sent to motors. (Can't exceed the byte)
		if (PID_internals[index][power] > PID_POWER_MAX) {
			PID_internals[index][power] = PID_POWER_MAX;
		} else if (PID_internals[index][power] < PID_POWER_MIN) {
			PID_internals[index][power] = PID_POWER_MIN;
		}

		// If the power level is below the deadband, increase it so that the
		// motor is actually moving toward its goal.
		if (PID_internals[index][power] > 0 && PID_internals[index][power] < PID_POWER_POSITIVE_DEADBAND[index]
				&& PID_POWER_POSITIVE_DEADBAND[index] != 0 && PID_internals[index][power] != 0) {
			PID_internals[index][power] = PID_POWER_POSITIVE_DEADBAND[index];
		} else if (PID_internals[index][power] < 0 && PID_internals[index][power] > PID_POWER_NEGATIVE_DEADBAND[index]
				&& PID_POWER_NEGATIVE_DEADBAND[index] != 0 && PID_internals[index][power] != 0) {
			PID_internals[index][power] = PID_POWER_NEGATIVE_DEADBAND[index];
		}

		// Set the motor power value.
		// Type cast to keep from wrongly setting the motors.
		if (index == MainLift) {
			motor[liftMotor1] = (int) PID_internals[index][power];
			motor[liftMotor2] = (int) PID_internals[index][power];
		} else if (index == ClawLift) {
			motor[clawLiftMotor] = (int) PID_internals[index][power];
		} else if (index == ClawWrist) {
			motor[clawWristMotor] = (int) PID_internals[index][power];
		}

		// Indicate that PID is struggling to meet its target.
		if (abs(PID_internals[index][error]) >= PID_OVERLOAD_THRESHOLD[index]
				&& (PID_internals[index][power] >= PID_POWER_POSITIVE_DEADBAND[index]
				|| PID_internals[index][power] <= PID_POWER_NEGATIVE_DEADBAND[index])) {
			if (PID_blinkIdOverloaded[index] == NULL) {
				PID_blinkIdOverloaded[index] = startBlinkTask(Severe, Solid);
			}
		} else if (PID_blinkIdOverloaded[index] != NULL) {
			stopBlinkTask(PID_blinkIdOverloaded[index]);
			PID_blinkIdOverloaded[index] = NULL;
		}

		// Shut the disabled blink task off.
		if (PID_blinkIdDisabled[index] != NULL) {
			stopBlinkTask(PID_blinkIdDisabled[index]);
			PID_blinkIdDisabled[index] = NULL;
		}
	} else {
		// Reset the variables.
		PID_internals[index][error] = 0;
		PID_internals[index][lastError] = 0;
		PID_internals[index][integral] = 0;
		PID_internals[index][derivative] = 0;
		PID_internals[index][power] = 0;
		// Reset the target so it doesn't jump when it is re-enabled.
		PID_target[index] = 0;
		if (PID_blinkIdDisabled[index] == NULL) {
			if (index == MainLift) {
				PID_blinkIdDisabled[index] = startBlinkTask(Warning, Slow);
			} else if (index == ClawLift) {
				PID_blinkIdDisabled[index] = startBlinkTask(Warning, Medium);
			} else if (index == ClawWrist) {
				PID_blinkIdDisabled[index] = startBlinkTask(Warning, Fast);
			}
			if (PID_blinkIdOverloaded[index] != NULL) {
				stopBlinkTask(PID_blinkIdOverloaded[index]);
				PID_blinkIdOverloaded[index] = NULL;
			}
		}
		// Be sure to stop the motor as well. (Externally once, rather than many times)
	}
}

task pidController() {
	while (true) {
		pidCycle(MainLift);
		pidCycle(ClawLift);
		pidCycle(ClawWrist);

		// Repeat at defined delta time, in Hertz.
		wait1Msec(1000 / PID_INTERVALS_PER_SECOND);
	}
}

task calibrateMainLift() {
	motor[liftMotor1] = -64;
	motor[liftMotor2] = -64;
	while (SensorValue[liftTouch] != 1) {
		// Wait...
		wait1Msec(10);
	}
	// Set the encoder to 0 for the PID loop.
	SensorValue[liftEncoder] = 0;

	// Find the max range of the lift. Usually somewhere around 2000.
	motor[liftMotor1] = 96;
	motor[liftMotor2] = 96;
	int lastEncoder = -10;
	while (SensorValue[liftEncoder] > lastEncoder) {
		lastEncoder = SensorValue[liftEncoder];
		wait1Msec(200);
	}
	motor[liftMotor1] = 0;
	motor[liftMotor2] = 0;
	PID_TARGET_MAX[MainLift] = SensorValue[liftEncoder];

	// Send to bottom.
	PID_target[MainLift] = 0;
}

task calibrateClawLift() {
	motor[clawLiftMotor] = -64;
	int lastEncoder = 99999;
	while (SensorValue[clawLiftEncoder] < lastEncoder) {
		lastEncoder = SensorValue[clawLiftEncoder];
		wait1Msec(200);
	}
	// Set the encoder to 0 for the PID loop.
	SensorValue[clawLiftEncoder] = 0;

	// Find the max range of the lift.
	motor[clawLiftMotor] = 96;
	lastEncoder = -10;
	while (SensorValue[clawLiftEncoder] > lastEncoder) {
		lastEncoder = SensorValue[clawLiftEncoder];
		wait1Msec(200);
	}
	motor[clawLiftMotor] = 0;
	PID_TARGET_MAX[ClawLift] = SensorValue[clawLiftEncoder];

	// Send to bottom.
	PID_target[ClawLift] = 0;
}

task calibrateClawWrist() {
	// Reset at the bottom, while simultaneously checking to see if the potentiometer is installed correctly.
	int lastPot = 99999;
	while (SensorValue[clawWristPot] != 0) {
		lastPot = SensorValue[clawWristPot];
		motor[clawWristMotor] = -64;
		wait1Msec(100);
		if (lastPot < SensorValue[clawWristPot]) {
			motor[clawWristMotor] = 0;
			writeDebugStreamLine("[PID]: The claw wrist potentiometer is installed incorrectly. Reverse the motor or reposition the potentiometer.");
			PID_ENABLED[ClawWrist] = false;
			startBlinkTask(Severe, Slow);
			break;
		}
	}
	motor[clawWristMotor] = 0;

	// Send to middle.
	PID_target[ClawWrist] = PID_TARGET_MAX[ClawWrist] / 2;
}

void pidCalibrate() {
	// Reset memory.
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 8; j++) {
			PID_internals[i][j] = 0;
		}
	}

	motor[clawMotor] = 127; // Close the claw.
	// Calibrate each individual system simultaneously.
	if (PID_ENABLED[MainLift] == true) {
		startTask(calibrateMainLift);
	}
	if (PID_ENABLED[ClawLift] == true) {
		startTask(calibrateClawLift);
	}
	if (PID_ENABLED[ClawWrist] == true) {
		startTask(calibrateClawWrist);
	}

	while (!(motor[liftMotor1] == 0 && motor[clawLiftMotor] == 0 && motor[clawWristMotor] == 0)) {
		// Wait until each system is calibrated.
		wait1Msec(100);
	}
	// Succesfully calibrated.
	motor[clawMotor] = -127; // Open the claw.

	// Enable PID loop.
	startTask(pidController);
	PID_target[MainLift] = 0;
}
