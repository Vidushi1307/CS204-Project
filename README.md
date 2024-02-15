# CS204-Project
How it works currently:
RtypeInstructions.txt has operations stored in the following format:

opname opcode func3 func7
	
demoInput.asm has input assembly code (only R type instructions so far).
demoOutput.asm has the corresponding machine code.
	
Rconvert.cpp has the main code so far. It contains all the functions.
	
Problem I don't yet know how to solve elegantly: 
How to tell what format (R,I,S,SB,U,UJ) an instruction is after reading it?
we will have to do this in order to parse the instruction correctly.
	

To be done:

Create similar code files for Itype, Stype, etc..

Write code that takes care of the assembler directives (.data, .byte, etc..)
	
Create a main code file that accesses all the R,I,S,SB,U,UJ files.
	
	
Things to clarify with sir:
do we have to deal with labels for branch, and jump instructions? Or assume integer immediate value is given?
