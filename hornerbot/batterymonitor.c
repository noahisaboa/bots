float bm_battery_level = (float)(nAvgBatteryLevel) / 1000; //Easy access for other parts of the code to shut down if neccesary

task battery_monitor() {
	bm_battery_level = (float)(nAvgBatteryLevel) / 1000; //battery level is 1000 less than the function returns
	
	//reset all lights before monitor is started
	SensorValue[greenled] = 0;
	SensorValue[yellowled] = 0;
	SensorValue[redled] = 0;
	while(true) {
		if(bm_battery_level < 5.5) { //Critial battery level
			SensorValue[greenled] = 0;
			SensorValue[yellowled] = 0;
			SensorValue[redled] = SensorValue[redled] ? 0:1; //invert the red light
		}
		else if(bm_battery_level < 6.0) {//Warning level 2 - almost critical
			SensorValue[greenled] = 0;
			SensorValue[yellowled] = 0;
			SensorValue[redled] = 1; //just a solid red light
		}
		else if(bm_battery_level <= 6.6) { //Warning
			SensorValue[greenled] = 0;
			SensorValue[redled] = 0;
			SensorValue[yellowled] = 1; //
		}
		else { //Battery level is fine
			SensorValue[yellowled] = 0;
			SensorValue[redled] = 0;
			SensorValue[greenled] = 1;
		}
		sleep(1000); //1000 milliseconds in a second
#ifdef DEBUG
	  writeDebugStreamLine("[Battery Monitor] The curent battery voltage is %f.0", bm_battery_level);
#endif
  }
}
