//////////////////////////////////////////////////////////////////////////////////////
//									Battery Module									//
//////////////////////////////////////////////////////////////////////////////////////
// This module quietly monitors the battery levels in the background, and when      //
// the voltage starts to get low, sets up a warning LED. It also flips a variable	//
// so that other modules can be alerted of the shortage of power, and accomodate	//
// to conserve more energy, at the cost of manuever quality.						//
//////////////////////////////////////////////////////////////////////////////////////

const float BAT_WARN = 6.6; // When to flash LED and output voltage to debug stream.
const float BAT_WARN2 = 6.0;
const float BAT_CRITICAL = 5.5;
const float BAT_INTERVALS_PER_SECOND = 10; // How often the monitor will check per second.
bool BAT_low = false; // To warn other modules of low battery. (Emergency consumption reducer.)
bool BAT_critical = false;

float BAT_level = (float)(nAvgBatteryLevel) / 1000; //Easy access for other parts of the code to shut down if neccesary

task BAT_monitor() {
    // Grabs an average sampling of the battery level then scales. (20 samples)
	BAT_level = (float)(nAvgBatteryLevel) / 1000; //battery level is 1000 less than the function returns
	
	// Interesting stats: http://www.vexrobotics.com/wiki/Quazar%27s_Battery_Load_Test
	
	//reset all lights before monitor is started
	SensorValue[PRT_ledg] = 0;
	SensorValue[PRT_ledy] = 0;
	SensorValue[PRT_ledR] = 0;
	short blinkid = 0;
	blinkid = LED_startBlinkTask(LED_LEVEL.UNASSIGNED, LED_RATE.UNASSIGNED); //Reserve a blink task
	while(true) {
	    
		if(!blinkid) blinkid = LED_startBlinkTask(LED_LEVEL.UNASSIGNED, LED_RATE.UNASSIGNED); //If we do not have a blink task, try to reserve one again
		
		if(BAT_level < BAT_CRITICAL) { //Critial battery level
			SensorValue[PRT_ledg] = 0;
			SensorValue[PRT_ledy] = 0;
			SensorValue[PRT_ledR] = SensorValue[PRT_ledR] ? 0:1; //Invert the red light
			BAT_critical = true;
			BAT_low = true;
			
			if(blinkid) LED_modBlinkTask(blinkid, LED_LEVEL.Severe, LED_RATE.Medium);
		}
		else if(BAT_level < BAT_WARN2) {//Warning level 2 - almost critical
			SensorValue[PRT_ledg] = 0;
			SensorValue[PRT_ledy] = 0;
			SensorValue[PRT_ledR] = 1; //just a solid red light
			BAT_low = true;
			BAT_critical = false;
			if(blinkid) LED_modBlinkTask(blinkid, LED_LEVEL.Severe, LED_RATE.Solid);
		}
		else if(BAT_level <= BAT_WARN) { //Warning
			SensorValue[PRT_ledg] = 0;
			SensorValue[PRT_ledR] = 0;
			SensorValue[PRT_ledy] = 1; //Solid yellow light
			BAT_low = true;
			BAT_critical = false;
			if(blinkid) LED_modBlinkTask(blinkid, LED_LEVEL.Warning, LED_RATE.Solid);
		}
		else { //Battery level is fine
			SensorValue[PRT_ledy] = 0;
			SensorValue[PRT_ledR] = 0;
			SensorValue[PRT_ledg] = 1;
			BAT_low = false;
			BAT_critical = false;
			if(blinkid) LED_modBlinkTask(blinkid, LED_LEVEL.Info, LED_RATE.Solid);
		}
		sleep(1000 / BAT_INTERVALS_PER_SECOND); //1000 milliseconds in a second
#ifdef DEBUG
	  writeDebugStreamLine("[Battery Monitor] The curent battery voltage is %f.0", BAT_level`);
#endif
  }
}

