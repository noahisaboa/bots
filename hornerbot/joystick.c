void joystick_control()
{
	//Loop Forever
	while(1 == 1)
	{
		//Remote Control Commands
		motor[frontRight] = vexRT[Ch3] - vexRT[Ch1] - vexRT[Ch4];
		motor[backRight] =  vexRT[Ch3] - vexRT[Ch1] + vexRT[Ch4];
		motor[frontLeft] = vexRT[Ch3] + vexRT[Ch1] + vexRT[Ch4];
		motor[backLeft] =  vexRT[Ch3] + vexRT[Ch1] - vexRT[Ch4];
		// Raise, lower or do not move arm
		if(vexRT[Btn6U] == 1)       	//If button 5U is pressed...
		{
			motor[bottomLift] = 127;    	//...raise the arm.
			motor[topLift] = 127;    	//...raise the arm.
		}
		else if(vexRT[Btn6D] == 1)  	//Else, if button 5D is pressed...
		{
			motor[bottomLift] = -127;   	//...lower the arm.
			motor[topLift] = -95;   	//...lower the arm.
		}
		else                      		//Else (neither button is pressed)...
		{
			motor[bottomLift] = 15;      	//...stop the arm.
			motor[topLift] = 15;      	//...stop the arm.
		}

		// Up, down linear lift
		if(vexRT[Btn5U] == 1)       	//If Button 6U is pressed...
		{
			motor[linearLift] = 127;  		//...close the gripper.
		}
		else if(vexRT[Btn5D] == 1)  	//Else, if button 6D is pressed...
		{
			motor[linearLift] = -127; 		//...open the gripper.
		}
		else                      		//Else (neither button is pressed)...
		{
			motor[linearLift] = 0;    		//...stop the gripper.
		}

		//Claw Rotation
		if(vexRT[Btn8R] == 1)
		{
			motor[clawOpenClose] = 127;
		}
		else if(vexRT[Btn8L] == 1)
		{
			motor[clawOpenClose] = -127;
		}
		else
		{
			motor[clawOpenClose] = 0;
		}

		//Claw Open and Close
		if(vexRT[Btn8D] == 1)
		{
			motor[clawRotate] = 127;
		}
		else if(vexRT[Btn8U] == 1)
		{
			motor[clawRotate] = -127;
		}
		else
		{
			motor[clawRotate] = 0;
		}
	}
}
