//////////////////////////////////////////////////////////////////////////////////////
//								   Autonomous Module	   							//
//////////////////////////////////////////////////////////////////////////////////////
// The miscellaneous autonomous functions that allow simpler control over the		//
// robot's individual modules.														//
//////////////////////////////////////////////////////////////////////////////////////

bool AUT_atTarget(PID_Type type) {
	if (abs(PID_internals[type][error]) <= PID_SYSTEM_HYSTERESIS[type]) {
		return true;
	}
	return false;
}

void AUT_setTarget (int height) {
	// Sets the PID target to height.
	// Height is determined by the PID_INPUT_SCALE value (likely inches).
	PID_target = height * PID_INPUT_SCALE;
}

void AUT_surge (int power, float time = 0) {
	// To move forward or backward. (Nautical term)
	// Positive is forward, negative is backward.
	motor[PRT_wheelFrontRight] = power;
	motor[PRT_wheelBackRight] = power;
	motor[PRT_wheelFrontLeft] = power;
	motor[PRT_wheelBackLeft] = power;
	if (time != 0) {
		wait1Msec(time);
		motor[PRT_wheelFrontRight] = 0;
		motor[PRT_wheelBackRight] = 0;
		motor[PRT_wheelFrontLeft] = 0;
		motor[PRT_wheelBackLeft] = 0;
	}
}

void AUT_strafe (int power, float time = 0) {
	// To move left or right without rotation.
	// Positive is to the right, negative is to the left.
	motor[PRT_wheelFrontRight] = -power;
	motor[PRT_wheelBackRight] = power;
	motor[PRT_wheelFrontLeft] = power;
	motor[PRT_wheelBackLeft] = -power;
	if (time != 0) {
		wait1Msec(time);
		motor[PRT_wheelFrontRight] = 0;
		motor[PRT_wheelBackRight] = 0;
		motor[PRT_wheelFrontLeft] = 0;
		motor[PRT_wheelBackLeft] = 0;
	}
}

void AUT_rotate (int power, float time = 0) {
	// To turn left or right without surge movement.
	// Positive is to the right, negative is to the left.
	motor[PRT_wheelFrontRight] = -power;
	motor[PRT_wheelBackRight] = -power;
	motor[PRT_wheelFrontLeft] = power;
	motor[PRT_wheelBackLeft] = power;
	if (time != 0) {
		wait1Msec(time);
		motor[PRT_wheelFrontRight] = 0;
		motor[PRT_wheelBackRight] = 0;
		motor[PRT_wheelFrontLeft] = 0;
		motor[PRT_wheelBackLeft] = 0;
	}
}

void AUT_elevate (int height) {
	PID_target = height;
}

void AUT_halt () {
	motor[PRT_wheelFrontRight] = 0;
	motor[PRT_wheelBackRight] = 0;
	motor[PRT_wheelFrontLeft] = 0;
	motor[PRT_wheelBackLeft] = 0;
}
