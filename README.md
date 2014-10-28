RoboRocks Triple Lift
====

Vex RobotC coding for **Skyrise**

## Coding Rules Before Committing.

1. Make sure it compiles, runs, and does not encounter immediate errors.
2. Follow naming conventions:
     - For global module variables, add the module prefix (i.e. `PID_<name>`). Place these at the top of the file.
     - Constants should have all letters capitalized and underscored (i.e. `POWER_MAX`).
     - Normal, editable variables should follow `camelCase`.
     - Variables for types, such as `enum`, should follow `PascalCase`.
     - Give variables meaningful names that are descriptive (i.e. `appleCount` rather than `ac`).
3. Indent properly to improve code readability. Please.
4. Reduce magic values in whatever way possible. Use variables more than just numbers in code.
5. Write code that is easily understandable across many programmers, including novices.
	 - Document your code thoroughly through the use of comments.
	 - Abstain from using more sophisticated programming rules, such as pointers, compiler shortcuts, or complicated data structures.
6. Break up large portions of code by modules, then include them in the `TripleLift.c` file. 

If you encounter a bug without a current solution, [submit a ticket](http://github.com/noahisaboa/bots/issues). 

## Goals

Ultimately, the robot's instructions should provide autonomous and dynamic problem solving.
Controls should be simple and intuitive, as the driver does not have an abundance of time to comprehend how to wrangle the robot.
The programming of the robot should not be bulky, or require an enormous amount of computing power to complete its task.

As a team, we should continually search for methods of reducing computing power, and improving the robot's overall intelligence aside from the user.
We should also stabilize the robot and its code so that it becomes more robust. After all, we don't want it to fail in the middle of a match.