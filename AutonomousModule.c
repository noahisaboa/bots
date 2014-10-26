bool atTarget(PID_Type type) {
	if (abs(PID_internals[type][error]) <= PID_SYSTEM_HYSTERESIS[type]) {
		return true;
	}
	return false;
}

void setTarget (int height) {
	// Sets the PID target to height.
	// Height is determined by the PID_INPUT_SCALE value (likely inches).
	PID_target = height * PID_INPUT_SCALE;
}

void surge (int power, float time = 0) {
	// To move forward or backward. (Nautical term)
	// Positive is forward, negative is backward.
	motor[wheelFrontRight] = power;
	motor[wheelBackRight] = power;
	motor[wheelFrontLeft] = power;
	motor[wheelBackLeft] = power;
	if (time != 0) {
		wait1Msec(time);
		motor[wheelFrontRight] = 0;
		motor[wheelBackRight] = 0;
		motor[wheelFrontLeft] = 0;
		motor[wheelBackLeft] = 0;
	}
}

void strafe (int power, float time = 0) {
	// To move left or right without rotation.
	// Positive is to the right, negative is to the left.
	motor[wheelFrontRight] = -power;
	motor[wheelBackRight] = power;
	motor[wheelFrontLeft] = power;
	motor[wheelBackLeft] = -power;
	if (time != 0) {
		wait1Msec(time);
		motor[wheelFrontRight] = 0;
		motor[wheelBackRight] = 0;
		motor[wheelFrontLeft] = 0;
		motor[wheelBackLeft] = 0;
	}
}

void rotate (int power, float time = 0) {
	// To turn left or right without surge movement.
	// Positive is to the right, negative is to the left.
	motor[wheelFrontRight] = -power;
	motor[wheelBackRight] = -power;
	motor[wheelFrontLeft] = power;
	motor[wheelBackLeft] = power;
	if (time != 0) {
		wait1Msec(time);
		motor[wheelFrontRight] = 0;
		motor[wheelBackRight] = 0;
		motor[wheelFrontLeft] = 0;
		motor[wheelBackLeft] = 0;
	}
}

void elevate (int height) {
	PID_target = height;
}

void halt () {
	motor[wheelFrontRight] = 0;
	motor[wheelBackRight] = 0;
	motor[wheelFrontLeft] = 0;
	motor[wheelBackLeft] = 0;
}
