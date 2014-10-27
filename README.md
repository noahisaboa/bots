bots
====

Vex RobotC coding for **Skyrise**

# Coding Rules
	1. Make sure it compiles, runs, and does not encounter immediate errors.
	2. Follow naming conventions:
		* a. For global module variables, add the module prefix (i.e. `PID_<name>`). Place these at the top of the file.
		* b. Constants should have all first letters capitalized and underscored (i.e. `POWER_MAX`).
		* c. Normal, editable variables should follow `camelCase`.
		* d. Variables for types, such as `enum`, should follow `PascalCase`.
		* e. Give variables meaningful names that are descriptive.
	3. Indent properly. Please.
	4. Reduce magic values in whatever way possible. Use variables more than just numbers in code.
	5. Document code thoroughly enough so that novice programmers are capable of understanding.
	6. Try not to use more sophisticated programming rules, such as pointers, or compiler shortcuts. Again, for the novices.
	7. Break up large portions of code by modules, then include them in the `TripleLift.c` file. 

If you encounter a bug without a current solution, [submit a ticket](http://github.com/noahisaboa/bots/issues). 

