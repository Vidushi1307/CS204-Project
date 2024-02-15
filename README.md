# CS204-Project
# How it works currently:

RtypeInstructions.txt has operations stored in the following format:

opname opcode func3 func7
	
demoInput.asm has input assembly code (only R type instructions so far).
demoOutput.asm has the corresponding machine code.

ASMtoMC.cpp has the main code. It contains all the functions.
	
To be done:
Create similar Instructions.txt for I,S,SB,U,UJ type instructions as well.

Create functions that read Instructions.txt for other types, and functions that convert instructions of those types to machine code.
bin2hex can be reused for any type.

Write code that takes care of the assembler directives (.data, .byte, etc..)
	
	
Things to clarify with sir:
do we have to deal with labels for branch, and jump instructions? Or assume integer immediate value is given?
