#include<iostream>
#include<fstream>
#include<map>
#include<string>
#include<algorithm>
#define INPUTFILE "demoInput.asm"
#define OUTPUTFILE "demoOutput.mc"
using namespace std;
map<string,string> dataInc, opcode, f3, f7, formatType;
map<string,int> labelAddress;
// dataInc : maps directive name to number of bytes to skip.
// opcode : maps instruction name to string corresponding to opcode.
// f3 : maps instruction name to string corresponding to func3.
// f7 : maps instruction name to string corresponding to func7.
// formatType : maps instruction name to its format (R,I,S,SB,U,UJ).
// labelAddress : maps label names to addresses.

string inst, opname, dirname, mc, conv = "0123456789ABCDEF", dataSegment;
// inst : instruction currently being parsed.
// opname : name of the operation currently being parsed.
// dirname : name of the assembler directive currently being parsed.
// mc : the machine code string.
//conv : string to aid conversion to hexadecimal.
// dataSegment : the entire data segment (because it has to be printed after the text segment).


int pc = 0, dc = 0x10000000;
// pc : program counter
// dc : data counter


//Function List
// 1. Common Functions.
string counterToHex(int counter); //Returns counter in hexadecimal.
string bin2hex(string s); //Converts binary machine code string to hexadecimal string.
void buildMaps(); //Calls all the other map building functions.
int bitCount(unsigned long long int x); //Counts the number of bits in x.
void skipDelimiters(); //Skips any spaces, commas, and closing parantheses before the next characters in the instruction. -- ' ' or ',' or ')'.


// 2. Text Segment Functions.
void buildRMaps(); //Creates and initializes maps related to R-type instructions.
string getName(); //Returns the name of the instruction.
string getReg(); //Returns the next register in the instruction.
void typeRmc();  //Creates the machine code for an R-type instruction.
void invalidInstruction(); //Informs user that the instruction does not exist in the database.
void labelAlreadyExists(string label); //Informs user that the label found was already previously used.
void formatMatcher(); //Calls the machine code creating functions of different formats.


// 3. Data Segment Functions.
string getDirectiveName(); //Returns the name of assembler directive.
void buildDirectiveMaps(); //Creates and initializes the dataInc map.
void invalidDirective(); //Informs user that the directive does not exist in the database.
void outOfRange(long long int x); //Informs user that the number entered is out of range.
string invalidString(string temp); //Informs user that the string entered is incorrect.
string getString(); //Reads next string from instruction.
void dataSegmentReader(istream& asmFile); //Reads the data segment of the program.



//Driver function.
int main()
{
	buildMaps();
	ifstream asmFile(INPUTFILE); //Opening input file.
	ofstream temp(OUTPUTFILE); temp.close(); //Clearing output file.
	ofstream mcFile(OUTPUTFILE, ios::app); //Opening output file.
	getline(asmFile,inst);
	skipDelimiters();
	while(!asmFile.eof())
	{
		if (inst[0] == '.') //If instruction might be a directive,
		{
			dirname = getDirectiveName();
			skipDelimiters();
			if (dataInc[dirname] != "") dataSegmentReader(asmFile);
			else invalidDirective(); //If directive is not valid.
			dirname = "";
		}
		else
		{
			opname = getName();
			skipDelimiters();
			if (formatType[opname] != "") formatMatcher();
			else invalidInstruction();			
			pc += 4;
			opname = "";
		}
		inst = "";
		getline(asmFile,inst); //Get next line from assembly code file.
	}
	mcFile << "-------End Of Code Segment-------" << endl; //Marking end of code segment. 
	mcFile << dataSegment;
	mcFile.close();
	asmFile.close();

// Printing the label addresses for checking.
//	for (auto it = labelAddress.begin(); it != labelAddress.end(); it++)
//	{
//		cout << it->first << " " << counterToHex(it->second) << endl;
//	}
	return 0;
}



/*-------------------------------------------------- COMMON FUNCTIONS --------------------------------------------------*/
//Function to convert integer pc to hexadecimal.
string counterToHex(int counter)
{
	string counterInHex;	
	int temp = counter;
	if (temp == 0) counterInHex = '0';
	
	//Converting to hex
	while (temp)
	{
		counterInHex += conv[temp%16];
		temp /= 16;
	}
	reverse(counterInHex.begin(), counterInHex.end());
	
	return "0x" + counterInHex;
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


void buildMaps()
{
	buildDirectiveMaps();
	buildRMaps();
//	buildIMaps();
//	buildSMaps();
	buildSBMaps();
	buildUMaps();
	buildUJMaps();
}


int bitCount(unsigned long long int x)
{
	int count = 0;
	while(x /= 2) count++;
	return count+1;
}


void skipDelimiters()
{
	while (inst.length() && (inst[0] == ' ' || inst[0] == ',' || inst[0] == ')')) inst.erase(0,1);
}


/*-------------------------------------------------- TEXT SEGMENT FUNCTIONS --------------------------------------------------*/


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


//Function that reads SBtypeInstructions.txt and stores opcode, func3 values into maps.
void buildSBMaps()
{
    ifstream iFile("SBtypeInstructions.txt");
    string temp,name;
    while(1)
    {
        iFile >> name;//reading operation name
        if (name == "end" || iFile.eof()) break;
        iFile >> temp;//reading opcode
        opcode[name]=temp;

        iFile >> temp;//reading funct3
        f3[name]=temp;

        iFile >> temp;//reading format type
        formatType[name]=temp;
    }
    iFile.close();
}

//Function that reads UtypeInstructions.txt and stores opcode values into maps.
void buildUMaps()
{
    ifstream iFile("UtypeInstructions.txt");
    string temp,name;
    while(1)
    {
        iFile >> name;//reading operation name
        if (name == "end" || iFile.eof()) break;
        
        iFile >> temp;//reading opcode
        opcode[name]=temp;

        iFile >> temp;//reading format type
        formatType[name]=temp;
    }
    iFile.close();
}

//Function that reads UJtypeInstructions.txt and stores opcode values into maps.
void buildUJMaps()
{
    ifstream iFile("UJtypeInstructions.txt");
    string temp,name;
    while(1)
    {
        iFile >> name;//reading operation name
        if (name == "end" || iFile.eof()) break;
        
        iFile >> temp;//reading opcode
        opcode[name]=temp;

        iFile >> temp;//reading format type
        formatType[name]=temp;
    }
    iFile.close();
}
//Getting the name of the instruction.
string getName()
{
	string name;
	while(inst.length() && inst[0] != ' ' && inst[0] != ',' && inst[0] != ':')
	{
		name += inst[0];
		inst.erase(0,1); //erasing the character read.
	}
	skipDelimiters();
	if (inst[0] == ':') //name was actually a label!
	{
		if (labelAddress[name]) labelAlreadyExists(name);
		else labelAddress[name] = pc;
		inst.erase(0,1);
		skipDelimiters();
		return getName();
	}
	return name;
}


//Getting the register name.
string getReg()
{
	string reg;
	if (inst[0] == '(') inst.erase(0,1); //getting rid of the potential '(' for load-store type of instructions.
	if (inst[0] == 'x') inst.erase(0,1); //getting rid of the 'x'.
	else return "";	
	
	//obtaining the register number as a string.
	while (inst.length() && inst[0] != ' ' && inst[0] != ',' && inst[0] != 'x' && inst[0] != '\n')
	{
		reg += inst[0];
		inst.erase(0,1);
	}
	skipDelimiters();

	//converting register number to int.
	int x = stoi(reg);
	reg = "";
	
	//converting the int to a 5 bit binary string.
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


void labelAlreadyExists(string label)
{
	ofstream mcFile(OUTPUTFILE, ios::app);
	mcFile << "The label " << label << ':' << " already exists at " << counterToHex(labelAddress[label]) << endl;
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


/*-------------------------------------------------- DATA SEGMENT FUNCTIONS --------------------------------------------------*/
//Extract name of directive from instruction
string getDirectiveName()
{
	if(inst[0] == '.') inst.erase(0,1); //Remove the '.'
	else
	{
		string label = "";
		while(inst.length() && inst[0] != ':' && inst[0] != ' ') //while you don't hit a delimiter,
		{
			label += inst[0]; //append to directive
			inst.erase(0,1);
		}
		if (inst.length() && inst[0] == ':') inst.erase(0,1);
		skipDelimiters();
		if (labelAddress[label]) labelAlreadyExists(label);
		else labelAddress[label] = dc; //adding the label to the map.
		return getDirectiveName();
	}
	string directive = "";
	while(inst.length() && inst[0] != ' ') //while you don't hit a delimiter,
	{
		directive += inst[0]; //append to directive
		inst.erase(0,1);
	}
	return directive; //return the name of the directive.
}


void buildDirectiveMaps()
{
	ifstream iFile("directives.txt");
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


void invalidDirective()
{
	ofstream mcFile(OUTPUTFILE, ios::app);
	mcFile << "Invalid directive: ." << dirname << endl;
	mcFile.close();
}


void outOfRange(long long int x)
{
	ofstream mcFile(OUTPUTFILE, ios::app);
	mcFile << "Number " << x << " is out of range for " << dirname << endl;
	mcFile.close();
}


string invalidString(string temp)
{
	ofstream mcFile(OUTPUTFILE, ios::app);
	mcFile << "Invalid string " << '"' << temp << endl;
	mcFile.close();
	return "";
}

//Reading next string from instruction.
string getString()
{
	string temp = "";
	while (inst.length() && inst[0] != '"')
	{
		temp += inst[0];
		inst.erase(0,1);
	}
	if (inst.length()) inst.erase(0,1); //Checking if the string ended with a double quote.
	else return invalidString(temp);
	return temp;
}


void dataSegmentReader(istream& asmFile)
{
	int increment = stoi(dataInc[dirname]); //Getting increment correspponding to directive.
	string num = "";
	if (increment < 0) //Increment is set as -1 for .text, .data, .string, and .asciiz
	{
		if (dirname == "text") return; //If .text, return back and continue with next instruction.
		else if (dirname == "data") //If .data, get the next directive.
		{
			getline(asmFile,inst);
			dirname = getDirectiveName();
			increment = stoi(dataInc[dirname]);
		}
		else if (dirname == "string" || dirname == "asciiz")
		{
			inst.erase(0,1); //Getting rid of the opening double quote ".
			num += getString();
			
			if (num != "") dataSegment += counterToHex(dc) + " " + num + '\n';
			dc += num.length();
			return;
		}
		else return;
	}
	if (increment == 0) invalidDirective(); //If .data is followed by an invalid directive.
	while (inst.length() && increment > 0)
	{
		skipDelimiters();
		//Read number to be stored.
		while(inst.length() && inst[0] != ' ')
		{
			num += inst[0];
			inst.erase(0,1);
		}
		
		//Convert to integer
//		cout << num << endl;
		long long int x = stoi(num);
		num = "";
		
		//Confirm number is in range.
		if (bitCount(abs(x)) > 8*increment)
		{
			outOfRange(x);
			continue;
		}
		
		//Convert number to hex string.
		for (int i = 0; i < 2*increment; i++)
		{
			num += conv[((x%16)+16)%16];
			x = x >> 4;
		}
		reverse(num.begin(), num.end());

		//Append to data segment.
		dataSegment += counterToHex(dc) + " 0x" + num + "\n";
		dc += increment;
		num = "";
	}
}

