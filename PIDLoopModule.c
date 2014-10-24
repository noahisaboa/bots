bool PID_ENABLED = true; // Non-constant allows for emergency override during run time.
const bool PID_INTEGRAL_ENABLED = false;
const bool PID_DERIVATIVE_ENABLED = false;
const int PID_POWER_MAX = 127;
const int PID_POWER_MIN = -127;
int PID_POWER_POSITIVE_DEADBAND = 32; // Amount of motor play that allows minimal movement.
int PID_POWER_NEGATIVE_DEADBAND = 0;
int PID_SYSTEM_HYSTERESIS = 8; // Solves for motor jogging without breaking PID.
const int PID_INTEGRAL_LIMIT = 50;
const int PID_INTERVALS_PER_SECOND = 100; // Essentially Hertz.

const float PID_SENSOR_SCALE = 1; // If scaling of the encoder is necessary.
const float PID_INPUT_SCALE = (1 / 77.09); // Set this to be INCH / ENCODER TICKS.
float PID_target = 0; // For setting the target height. Do not touch.
float PID_actual = 0; // For detecting actual height. Do not touch.
float PID_SIGNAL_GENERATOR_MAX = 0.96; // Percentage value for horse and carrot.
const float PID_TARGET_MAX = 2000; // Max height from the origin.

const int PID_OVERLOAD_THRESHOLD = 450; // If the PID is behind this much, activates overload LED.
short PID_blinkIdDisabled; // Placeholders for blink task ID's
short PID_blinkIdOverloaded;

// PID Loop constants for Proportional, Integral, and Derivative.
float PID_KP = 0.80;
const float PID_KI = 0.00;
const float PID_KD = 0.00;

float PID_CONTROL_INCREMENTER_UP = 0.225; // How much controller button should add/subtract to the target.
float PID_CONTROL_INCREMENTER_DOWN = 0.325;

task pidController() {
	float filteredTarget = 0;
	float power = 0;
	float error = 0;
	float lastError = 0;
	float integral = 0;
	float derivative = 0;
	float lastTarget = 0;
	float lastActual = 0;

	// Reset the sensor value. (Pre-Autonomous should set lift to bottom)
	SensorValue[liftEncoder] = 0;

	while (true) {
		if (PID_ENABLED) {
			// Grab the actual sensor value, then scale it to real world units.
			lastActual = PID_actual;
			PID_actual = SensorValue[liftEncoder] * PID_SENSOR_SCALE;

			// Target filter using a signal generator (horse and carrot method).
			filteredTarget = PID_SIGNAL_GENERATOR_MAX * lastTarget + (1.00 - PID_SIGNAL_GENERATOR_MAX) * PID_target;
			lastTarget = filteredTarget;

			if (PID_target > PID_TARGET_MAX) {
				PID_target = PID_TARGET_MAX;
			}

			// Calculate the error.
			error = PID_actual - filteredTarget;
			if (abs(error) < PID_SYSTEM_HYSTERESIS) {
				error = 0;
			}

			/*if (PID_actual == 0 && (SensorValue[liftTouch] != 1)) {
				// If we are supposedly at 0, and are not touching the bottom,
				// increase the error to meet the bottom.
				error = -15;
			} else*/ if (error < 0 && SensorValue[liftTouch] == 1) {
				// If we are at the bottom, and we are supposedly not,
				// reset the error, actual, and encoder.
				error = 0;
				PID_actual = 0;
				SensorValue[liftEncoder] = 0;
			}

			if (PID_INTEGRAL_ENABLED) {
				// Accumulate error to fill the gap.
				if (abs(error) < PID_INTEGRAL_LIMIT) {
					integral += error;
				}
				if(PID_KI == 0)
				    integral = 0;
			}
			else integral = 0;

			if (PID_DERIVATIVE_ENABLED) {
				// Predict future change via derivative.
				derivative = (error - lastError);
				lastError = error;
			} else {
				derivative = 0;
			}

			// Calculate the power necessary to correct the found error.
			// Negations are to keep error and power values in a logical context.
			power = -1 * ((PID_KP * error) + (PID_KI * integral)  + (PID_KD * derivative));

			// Limit the power sent to motors. (Can't exceed the byte)
			if (power > PID_POWER_MAX) {
				power = PID_POWER_MAX;
			} else if (power < PID_POWER_MIN) {
				power = PID_POWER_MIN;
			}

			// If the power level is below the deadband, increase it so that the
			// motor is actually moving toward its goal.
			if (power > 0 && power < PID_POWER_POSITIVE_DEADBAND && PID_POWER_POSITIVE_DEADBAND != 0 && power != 0) {
				power = PID_POWER_POSITIVE_DEADBAND;
			} else if (power < 0 && power > PID_POWER_NEGATIVE_DEADBAND && PID_POWER_NEGATIVE_DEADBAND != 0 && power != 0) {
				power = PID_POWER_NEGATIVE_DEADBAND;
			} /*else if (power == 0 || error == 0) {
				// Apply hold power.
				power = PID_POWER_POSITIVE_DEADBAND  / 2;
			}*/

			// Set the motor power value.
			// Type cast to keep from wrongly setting the motors.
			// Negations are to keep error and power values in a logical context.
			motor[liftMotor1] = (int) power;
			motor[liftMotor2] = (int) power;

			// Indicate that PID is struggling to meet its target.
			if (abs(error) >= PID_OVERLOAD_THRESHOLD && (power >= PID_POWER_POSITIVE_DEADBAND
				|| power <= PID_POWER_NEGATIVE_DEADBAND)) {
				if (PID_blinkIdOverloaded == NULL) {
					PID_blinkIdOverloaded = startBlinkTask(Severe, Solid);
				}
			} else if (PID_blinkIdOverloaded != NULL) {
				stopBlinkTask(PID_blinkIdOverloaded);
				PID_blinkIdOverloaded = NULL;
			}

			if (PID_blinkIdDisabled != NULL) {
				stopBlinkTask(PID_blinkIdDisabled);
				PID_blinkIdDisabled = NULL;
			}
		} else {
			// Reset the variables.
			error = 0;
			lastError = 0;
			integral = 0;
			derivative = 0;
			power = 0;
			// Reset the target so it doesn't jump when it is re-enabled.
			PID_target = 0;
			if (PID_blinkIdDisabled == NULL) {
				PID_blinkIdDisabled = startBlinkTask(Warning, Slow);
				if (PID_blinkIdOverloaded != NULL) {
					stopBlinkTask(PID_blinkIdOverloaded);
					PID_blinkIdOverloaded = NULL;
				}
			}
			// Be sure to stop the motor as well. (Externally once, rather than many times)
		}

		// Repeat at defined delta time, in Hertz.
		wait1Msec(1000 / PID_INTERVALS_PER_SECOND);
	}
}
