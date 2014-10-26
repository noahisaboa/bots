// Enum values for different blink task settings.
enum Level {Info = 1, Warning, Severe, UNASSIGNED = 99};
enum Rate {Solid = 1, Slow, Medium, Fast, UNASSIGNED = 99};

// LED_Task structure, comparable to OOP's Class.
typedef struct {
	Level level;
	Rate rate;
} LED_Task;

LED_Task tasks[15];

task blink() {
	// Setup the array.
	// "(sizeof(tasks) / 4)" is a way to find array size. Each struct is worth 4 bytes.
	for (int i = 0; i < (sizeof(tasks) / 4); i++) {
		tasks[i].level = UNASSIGNED;
		tasks[i].rate = UNASSIGNED;
	}
	// Reset LED's.
	// Ignore the errors these lines generate.
	SensorValue[LEDG] = 0;
	SensorValue[LEDY] = 0;
	SensorValue[LEDR] = 0;
	// Loop through each setting and apply appropriately.
	// If there are overlapping blink tasks, more recent ones (likely more important), will be shown.
	while (true) {
		for (int cycle = 1; cycle < (6 + 1); cycle++) {
			for (int i = (sizeof(tasks) / 4) - 1; i > -1; i--) {
				if (tasks[i].level != UNASSIGNED && tasks[i].rate != UNASSIGNED) {
					short index;
					if (tasks[i].level == Info) {
						index = LEDG;
					} else if (tasks[i].level == Warning) {
						index = LEDY;
					} else if (tasks[i].level == Severe) {
						index = LEDR;
					}

					if (cycle == 1) {
						SensorValue[index] = 1;
					} else if (cycle == 3 && tasks[i].rate == Fast) {
						SensorValue[index] = 1;
					} else if (cycle == 4 && tasks[i].rate == Medium) {
						SensorValue[index] = 1;
					} else if (cycle == 5 && tasks[i].rate == Fast) {
						SensorValue[index] = 1;
					} else if (tasks[i].rate == Solid) {
						SensorValue[index] = 1;
					} else {
						SensorValue[index] = 0;
					}
				}
			}
			wait1Msec(1000 / 6);
		}
	}
}

short startBlinkTask(Level level, Rate rate) {
	// Find an empty slot, then fill it with a new task.
	// Return identifier.
	for (int i = (sizeof(tasks) / 4) - 1; i > -1; i--) {
		if (tasks[i].level == UNASSIGNED && tasks[i].rate == UNASSIGNED) {
			tasks[i].level = level;
			tasks[i].rate = rate;
			return i;
		}
	}
	// If there are too many tasks, return null.
	return NULL;
}

bool stopBlinkTask(short identifier) {
	// Check to see if given identifier is real, then clear it.
	// Return with success value.
	if (tasks[identifier].level != UNASSIGNED && tasks[identifier].rate != UNASSIGNED) {
		switch (tasks[identifier].level) {
			case Info:
				SensorValue[LEDG] = 0;
				break;
			case Warning:
				SensorValue[LEDY] = 0;
				break;
			case Severe:
				SensorValue[LEDR] = 0;
				break;
		}
		tasks[identifier].level = UNASSIGNED;
		tasks[identifier].rate = UNASSIGNED;
		return true;
	} else {
		return false;
	}
}

void clearBlinkTasks() {
	for (int i = 0; i < (sizeof(tasks) / 4); i++) {
		tasks[i].level = UNASSIGNED;
		tasks[i].rate = UNASSIGNED;
	}
	SensorValue[LEDG] = 0;
	SensorValue[LEDY] = 0;
	SensorValue[LEDR] = 0;
}
