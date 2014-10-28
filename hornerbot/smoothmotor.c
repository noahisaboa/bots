//TODO: DOCUMENT ALL CODE

//The maximum power that the motor is allowed to recieve
int MM_MAX_POWER = 127;
//The minimum power that the motor is allowed to recieve
int MM_MIN_POWER = -127;

//How far away the error should be before starting to slow the motor when the position is increasing
int MM_SLOW_AT_POS_INC = 150;

//The rate to multiply the output by when within SLOW_AT below the target
float MM_SLOW_X_RATE_POS_INC = 0.5;

//
int MM_SLOW_AT_POS_DEC = 500;

//
float MM_SLOW_X_RATE_POS_DEC = 0.5;

//The acceptable distance from the target (will stop when in the range)
int MM_ACCEPTABLE_ERROR = 5;

//The name configured in the sensor setup for the motor
#define MM_OUTPUT linearLift

//The name of the shaft encoder set in the sensors setup
#define MM_INPUT liftEncoder

//How many times per second to compute the motor power
int MM_ITER_PER_SECOND = 50;

//The minimum power required to make the motor move in the positive direction
int MM_MIN_MOTOR_MOVEMENT_POWER_POS = 28;

//The minimum power required to make the motor move in the negative direction (must be negative)
int MM_MIN_MOTOR_MOVEMENT_POWER_NEG = -18;

//Define the shaft encoder for easy use in the program
#define mm_position SensorValue[MM_INPUT]

//Define the motor for easy use in the program
#define mm_output motor[MM_OUTPUT]

#ifdef DEBUG
//Easy way to see the value of the sensor in the variables window
int mm_sensor;

//Easy way to see the motor's power in the variables window
int mm_motorpower;

#endif
//int error;

//int lasterror;

int mm_target = 0;

int mm_loopnum = 0;

bool within(int pos, int tar, int range) {
	return (pos < tar && pos > tar - range) || (pos > tar && pos < tar + range) || (pos == tar);
}

void mm_do_motorloop() {
	int error = mm_target - mm_position;
	float power = error;

	if(within(mm_position, mm_target, MM_ACCEPTABLE_ERROR)) {
		power = 0;
		goto comp_complete;
  }

	if(power > MM_MAX_POWER) {
		power = MM_MAX_POWER;
		goto comp_complete;
  }
	if(power < MM_MIN_POWER) {
    power = MM_MIN_POWER;
    goto comp_complete;
  }

  if(error > 0 && within(mm_position, mm_target, MM_SLOW_AT_POS_INC)) {
  	power *= MM_SLOW_X_RATE_POS_INC;
  }
  else if(error < 0 && within(mm_position, mm_target, MM_SLOW_AT_POS_DEC)) {
  	power *= MM_SLOW_X_RATE_POS_DEC;
  }

	comp_complete:
  if(MM_MIN_MOTOR_MOVEMENT_POWER_NEG >= 0) MM_MIN_MOTOR_MOVEMENT_POWER_NEG = -1;
  if(MM_MIN_MOTOR_MOVEMENT_POWER_POS <= 0) MM_MIN_MOTOR_MOVEMENT_POWER_POS = -1;

	if(power < 0 && power > MM_MIN_MOTOR_MOVEMENT_POWER_NEG) power = MM_MIN_MOTOR_MOVEMENT_POWER_NEG;
	if(power > 0 && power < MM_MIN_MOTOR_MOVEMENT_POWER_POS) power = MM_MIN_MOTOR_MOVEMENT_POWER_POS;

	mm_output = power > 0 ? ceil(power):floor(power);
#ifdef DEBUG
	mm_motorpower = power;
	mm_sensor = mm_position;
#endif

#ifdef DEBUG
  if(mm_loopnum == MM_ITER_PER_SECOND) {
    writeDebugStreamLine("[Motor Power] Current Motor Power is %i", mm_motorpower);
    writeDebugStreamLine("");
    writeDebugStreamLine("[Motor Power] Current Target is %i", mm_target);
    writeDebugStreamLine("[Motor Power] Current Position is %i", mm_sensor);
    writeDebugStreamLine("[Motor Power] Current Error is %i", abs(error));
    mm_loopnum = -1;
  }
  mm_loopnum++;
#endif
	sleep(1000 / MM_ITER_PER_SECOND);
}

task motor_movement() {
	while(true) {
		mm_do_motorloop();
  }
}
