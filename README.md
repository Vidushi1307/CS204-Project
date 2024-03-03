# CS204-Project
# How it works currently:

RtypeInstructions.txt has operations stored in the following format:

opname opcode func3 func7 format
	
demoInput.asm has input assembly code (only R type instructions so far).
demoOutput.asm has the corresponding machine code (with messages displayed where needed).

ASMtoMC.cpp has the main code. It contains all the functions.

I plan on later splitting up all the functions into different .cpp files (according to format type)
and only have the main() function in ASMtoMC.cpp.
	
To be done:
Create similar Instructions.txt for I,S,SB,U,UJ type instructions as well.

Create functions that read Instructions.txt for other types, and functions that convert instructions of those types to machine code.
bin2hex, getReg, etc.. can be reused for any other instruction types as well.
