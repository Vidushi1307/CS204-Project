#include<iostream>
#include<fstream>
#include<map>
#include<string>
#include<algorithm>
#define INPUTFILE "demoInput.asm"
#define OUTPUTFILE "demoOutput.mc"
using namespace std;
map<string,string> dataInc, opcode, f3, f7, formatType;
// dataInc : maps directive name to number of bytes to skip.
// opcaode : maps instruction name to string corresponding to opcode.
// f3 : maps instruction name to string corresponding to func3.
// f7 : maps instruction name to string corresponding to func7.
// formatType : maps instruction name to its format (R,I,S,SB,U,UJ).


string inst, opname, dirname, mc, conv = "0123456789ABCDEF", dataSegment;
// inst : instruction currently being parsed.
// opname : name of the operation currently being parsed.
// dirname : name of the assembler directive currently being parsed.
// mc : the machine code string.
//conv : string to aid conversion to hexadecimal.
// dataSegment : the entire data segment (because it has to be printed after the text segment).


int pc = 0, dc = 0;
// pc : program counter
// dc : data counter


//Extract name of directive from instruction
string getDirectiveName()
{
	inst.erase(0,1); //Remove the '.'
	string directive = "";
	while(inst.length() && inst[0] != ' ') //while you don't hit a delimiter,
	{
		directive += inst[0]; //append to directive
		inst.erase(0,1);
	}
	return directive; //return the name of the directive.
}


//Function to convert integer pc to hexadecimal.
string counterToHex(int pc)
{
	string pcInHex;	
	int temp = pc;
	if (temp == 0) pcInHex = '0';
	
	//Converting to hex
	while (temp)
	{
		pcInHex += conv[temp%16];
		temp /= 16;
	}
	reverse(pcInHex.begin(), pcInHex.end());
	
	return "0x" + pcInHex;
}


//Function to convert binary string of machine code into hexadecimal.
string bin2hex(string s)
{
	string hex;
	for (int i = 0; i < s.size();)
	{
		int num = 0;
		if (s[i++] == '1') num += 8;
		if (s[i++] == '1') num += 4;
		if (s[i++] == '1') num += 2;
		if (s[i++] == '1') num += 1;
		hex += conv[num];
	}
	return hex;
}

//Function that reads RtypeInstructions.txt and stores opcode, func3, func7 values into maps.
void buildRMaps()
{
	ifstream iFile("RtypeInstructions.txt");
	string temp, name;
	while(1)
	{
		iFile >> name; //Reading operation name.
		if (name == "end" || iFile.eof()) break;
		iFile >> temp; //Reading opcode.
		opcode[name] = temp;
		
		iFile >> temp; //Reading func3.
		f3[name] = temp;
		
		iFile >> temp; //Reading func7.
		f7[name] = temp;
		
		iFile >> temp; //Reading formatType.
		formatType[name] = temp;
	}
	iFile.close();
}


void buildDirectiveMaps()
{
	ifstream iFile("RtypeInstructions.txt");
	string increment, name;
	while(1)
	{
		iFile >> name; //Reading operation name.
		if (name == "end" || iFile.eof()) break;
		iFile >> increment; //Reading increment.
		dataInc[name] = increment;
	}
	iFile.close();
}


void buildMaps()
{
	buildDirectiveMaps();
	buildRMaps();
//	buildIMaps();
//	buildSMaps();
//	buildSBMaps();
//	buildUMaps();
//	buildUJMaps();
}


string getName()
{
	string name;
	while(inst.length() && inst[0] != ' ' && inst[0] != ',')
	{
		name += inst[0];
		inst.erase(0,1);
//		parserIndex++;
	}
	return name;
}


string getReg()
{
	string reg;
	if (inst[0] == 'x') inst.erase(0,1); //getting rid of the 'x'.
	else return "";	
	
	//obtaining the register number as a string.
	while (inst.length() && inst[0] != ' ' && inst[0] != ',' && inst[0] != 'x' && inst[0] != '\n')
	{
		reg += inst[0];
		inst.erase(0,1);
	}
	while (inst.length() && (inst[0] == ' ' || inst[0] == ',')) inst.erase(0,1);

	//converting register number to int.
	int x = stoi(reg);
	reg = "";
	
	//converting the int to a 5 bit binary number.
	for (int i = 0; i < 5; i++)
	{
		if (x&1) reg += '1';
		else reg += '0';
		x /= 2;
	}
	reverse(reg.begin(),reg.end()); 

	return reg;
}


//Getting all the fields and creating the machine code from them, for R type instruction.
void typeRmc()
{
	//Extracting the registers as binary strings.
	string rd,r1,r2;
	rd = getReg();
	r1 = getReg();
	r2 = getReg();
	ofstream mcFile(OUTPUTFILE, ios::app); //appending to the output file

	mc = f7[opname] + r2 + r1 + f3[opname] + rd + opcode[opname]; //getting the binary machine code.
	mc = bin2hex(mc); //converting the machine code to hex.

	mcFile << counterToHex(pc) << " " << "0x" << mc << endl;
	mcFile.close();
}


void invalidInstruction()
{
	ofstream mcFile(OUTPUTFILE, ios::app);
	mcFile << "Invalid instruction: " << opname << endl;
	mcFile.close();
	pc -= 4; //decrementing pc to account for the automatic increment.
}


string getData(long int n, int increment)
{
	string num = "";
	cout << n << " " << (1<<(increment)*8)-1 << endl;
	if (abs(n) > (1<<(increment*8))-1) num = "Number outside expected range";
	else
	{
		while (n)
		{
			if (n&1) num += '1';
			else num += '0';
			n = n>>1;
		}
		reverse(num.begin(),num.end());
		num = bin2hex(num);
	}
	num = counterToHex(dc) + " " + num;
	dc += increment;
	return num;
}


void dataSegmentReader(istream& asmFile)
{
	if (dirname == "text") return;
	if (dirname == "data")
	{
		getline(asmFile,inst);
		dirname = getDirectiveName();
	}
//	while (1)
//	{
//		
//	}
//	ifstream asmFile(INPUTFILE);
/*	while(1)
	{
		int increment = 0;
		string directive = "", data = "";
		asmFile >> directive;
		if (directive == ".text") return;
		if (directive == ".byte") increment = 1;
		else if (directive == ".half") increment = 2;
		else if (directive == ".word") increment = 4;
		else if (directive == ".double") increment = 8;
		else if (directive == ".string" || directive == ".asciiz") ;
		else invalidInstruction(directive);
		getline(asmFile, data);
		if (data[0] == '\n') continue; //If user doesn't enter any data after .byte or .half or ... then move on.
		
		for (int i = 0; increment && i < data.length(); i++)
		{
			string num = "";
			while (data[i] == ' ' || data[i] == '\t') i++;
			while (data[i] != ' ' && data[i] != '\t') num += data[i++];
			long int n = stoi(num);
			dataSegment += getData(n, increment) + '\n';
		}
		
		if (asmFile.eof()) return asmFile;
	}
*/
}


void formatMatcher()
{
	if (formatType[opname] == "R") typeRmc(); //Creating machine code for type R.
	else if (formatType[opname]=="I") ; //typeImc(inst);
	else if (formatType[opname]=="S") ; //typeSmc(inst);
	else if (formatType[opname]=="SB") ; //typeSBmc(inst);
	else if (formatType[opname]=="U") ; //typeUmc(inst);
	else if (formatType[opname]=="UJ") ; //typeUJmc(inst);
}


int main()
{
	buildMaps();
	ifstream asmFile(INPUTFILE);
	ofstream temp(OUTPUTFILE); temp.close(); //Clearing output file.
	ofstream mcFile(OUTPUTFILE, ios::app);
	getline(asmFile,inst);
	while(!asmFile.eof())
	{
		if (inst[0] == '.')
		{
			dirname = getDirectiveName();
			while (inst.length() && inst[0] == ' ') inst.erase(0,1);
			if (dataInc[dirname] != "") dataSegmentReader(asmFile);
		//	else invalidDirective();
			dirname = "";
		}
		else
		{
			opname = getName();
			while (inst[0] == ' ' || inst[0] == ',' && inst.length()) inst.erase(0,1);
			if (formatType[opname] != "") formatMatcher();
			else invalidInstruction();			
			pc += 4;
			opname = "";
		}
		inst = "";
		getline(asmFile,inst); //user input of instruction.
	}
	mcFile << "-------End Of Code Segment-------" << endl; //Marking end of code segment. 
	mcFile << dataSegment;
	mcFile.close();
	asmFile.close();
	return 0;
}
