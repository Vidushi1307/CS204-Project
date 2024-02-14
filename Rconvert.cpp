#include<iostream>
#include<fstream>
#include<map>
#include<string>
#include<algorithm>
using namespace std;
map<string,string> opcode, f3, f7;
string opname,r1,r2,rd,mc,conv = "0123456789ABCDEF";
int pc = 0, parserIndex = 0;

//Function to convert integer pc to hexadecimal.
string formatPC(int pc)
{
	string pcInHex;	
	int temp = pc;
	if (temp == 0) pcInHex = '0';
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
void buildRmaps()
{
	ifstream iFile("RtypeInstructions.txt");
	string temp, name;
	while(1)
	{
		iFile >> name;
		iFile >> temp;
		opcode[name] = temp;
		iFile >> temp;
		f3[name] = temp;
		iFile >> temp;
		f7[name] = temp;
		if (iFile.eof()) break;
	}
	iFile.close();
}


string getName(string inst)
{
	string name;
	while(inst[parserIndex] != ' ' && inst[parserIndex] != ',')
	{
		name += inst[parserIndex];
		parserIndex++;
	}
	return name;
}


string getReg(string inst)
{
	string reg;
	//Ignoring the delimiters space, comma, and letter 'x' (in front of register number).
	while (inst[parserIndex] == ' ' || inst[parserIndex] == ',' || inst[parserIndex] == 'x') parserIndex++;
	
	//obtaining the register number as a string.
	while (inst[parserIndex] != ' ' && inst[parserIndex] != ',' && inst[parserIndex] != 'x' && inst[parserIndex] != '\n')
	{
		reg += inst[parserIndex];
		parserIndex++;
	}
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
void typeRmc(string inst)
{
	opname = getName(inst);
	rd = getReg(inst);
	r1 = getReg(inst);
	r2 = getReg(inst);
	parserIndex = 0;

	ofstream mcFile("demoOutput.mc", ios::app);
	mc = f7[opname] + r2 + r1 + f3[opname] + rd + opcode[opname];
	mc = bin2hex(mc);
	mcFile << formatPC(pc) << " " << "0x" << mc << endl;
	mcFile.close();
}


int main()
{
	string inst;
	buildRmaps();
	ifstream asmFile("demoInput.asm");
	ofstream temp("demoOutput.mc"); temp.close();
	ofstream mcFile("demoOutput.mc", ios::app);
	while(1)
	{
		inst = "";
		getline(asmFile,inst); //user input of instruction.
		if (asmFile.eof()) break;
		typeRmc(inst); //Creating machine code for type R.
		pc += 4;
		opname = "";
		r1 = "";
		r2 = "";
		rd = "";
	}
	mcFile << "-------End Of Code Segment-------" << endl; //Marking end of code segment. 
	mcFile.close();
	asmFile.close();
	return 0;
}