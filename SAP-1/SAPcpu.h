#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <iterator> 
#include <map>
#include <bitset>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "ssdisp.h" // Output display


// HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
void throwError(std::string e);

class SAPcpu
{
private: /// Variables
	std::string codeFile;
	std::ifstream code;
	std::string exec;
	int clockrate = 1000; // Hz

	uint8_t wbus	= 0b0; // main bus

	uint8_t ALU		= 0b0; // Arithmetic Logic Unit

	uint8_t RAM[16] = {	}; // 16 word, 8 bit array

	// Regs
	uint8_t rega	= 0b0; // accumulator
	uint8_t regb	= 0b0; // buffer register
	uint8_t regc	= 0b0; // program register
	uint8_t MAR		= 0b0; // memory address register
	uint8_t DISP	= 0b0; // displayer regiter

	// PC
	uint8_t PC		= 0b0; // program counter

	bool haltFlag	= false;

	std::string state = "null";
	std::string op = "null";


private: /// Consts and funcs

	// Opcode
	struct operation
	{
		std::string mnemonic;
		uint8_t opc;
		bool args;
	};
	operation opcode[6] = { {"MOV", 0b11, true}, {"LDA", 0b0, true}, { "ADD", 0b1, true }, { "SUB", 0b10, true }, { "OUT", 0b1110, false }, { "HLT", 0b1111, false } };
	operation findOpcode(std::string _mnemonic)
	{
		for (uint8_t i = 0; i < std::size(opcode); i++)
			if (opcode[i].mnemonic == _mnemonic)
				return opcode[i];
		return opcode[4];
	}

	void parse() // translate .sap lang into binary and write to ram
	{
		operation cmd = findOpcode(exec.substr(0, 3));
		RAM[PC] = cmd.opc << 4;
		if (cmd.args)
		{
			if (cmd.opc == 0b11)
			{
				uint8_t addr = std::stoi(exec.substr(4, 4), (size_t *)nullptr, 2) & 0b1111;
				uint8_t val = std::stoi(exec.substr(9, 8), (size_t *)nullptr, 2) & 0b11111111;
				RAM[addr] = val;
				return;
			}
			
			RAM[PC] += std::stoi(exec.substr(3, exec.size()), (size_t *)nullptr, 2) & 0b1111;
		}
		PC++;
	}

	void loadWBUS(uint8_t _arg)
	{
		wbus = _arg;
		state = "Loading WBUS";
		out();
	}
	void busPC()
	{
		wbus = PC;
		state = "Loading PC to bus";
		out();
	}
	void loadMAR()
	{
		MAR = wbus & 0b1111;
		state = "Loading MAR";
		out();
	}
	void loadRAM()
	{
		RAM[MAR & 0b1111] = wbus;
		state = "Loading RAM";
		out();
	}
	void busRAM()
	{
		wbus = RAM[MAR & 0b1111];
		state = "Loading RAM to bus";
		out();
	}
	void loadA()
	{
		rega = wbus;
		state = "Loading reg A";
		out();
	}
	void busA()
	{
		wbus = rega;
		state = "Loading reg A to bus";
		out();
	}
	void loadB()
	{
		regb = wbus;
		state = "Loading reg B";
		out();
	}
	void loadC()
	{
		regc = wbus;
		state = "Loading reg C";
		out();
	}
	void busC()
	{
		wbus = regc & 0b1111;
		state = "Loading reg C to bus";
		out();
	}
	void busALU()
	{
		wbus = ALU;
		state = "Loading ALU to bus";
		out();
	}
	void loadDISP()
	{
		DISP = wbus;
		state = "Loading reg DISP";
		out();
	}

	// Instruction set
	void ADD()
	{
		busC();
		loadMAR();
		busRAM();
		loadB();
		ALU = rega + regb;
		busALU();
		loadA();
		PC++;
		op = "ADD " + std::to_string(regb);
		out();
	}
	void SUB()
	{
		busC();
		loadMAR();
		busRAM();
		loadB();
		ALU = rega - regb;
		busALU();
		loadA();
		PC++;
		op = "SUB " + std::to_string(regb);
		out();
	}
	void LDA()
	{
		busC();
		loadMAR();
		busRAM();
		loadA();
		PC++;
		op = "LDA " + std::to_string(rega);
		out();
	}
	void OUTPUT()
	{
		busA();
		loadDISP();
		PC++;
		op = "OUT";
		out();
	}
	void HLT()
	{
		haltFlag = true;
		PC++;
		op = "HLT";
		out();
	}

public:
	bool bin = false;

	SAPcpu() {}
	SAPcpu(std::string _codeFile) : codeFile(_codeFile)
	{
		code.open(codeFile);
		if (!code.is_open())
		{
			throwError("Default file not found...");
			codeFile = "";
			code.close();
		}
	}
	~SAPcpu() {}

	bool openFile(std::string _codeFile)
	{
		code.close();
		code.open(_codeFile);

		if (code.is_open())
		{
			std::cout << "Opened file: " << _codeFile << '\n';
			codeFile = _codeFile;
			return 1;
		}
		else
		{
			throwError("File not found...");
			codeFile = "";
			code.close();
			return 0;
		}
		return 0;
	}

	void closeFile()
	{
		code.close();
		codeFile = "";
		std::cout << "File closed...\n";
	}
	void setClockrate(int freq)
	{
		clockrate = freq;
	}
	std::string printLastCommand()
	{
		return exec;
	}

	void run()
	{
		if (code.is_open())
		{
			PC = 0;
			while (!code.eof()) // Autoparse
			{
				state = "Parsing";
				out();
				std::getline(code, exec);	
				parse();
				sleep(1000 / clockrate);
			}
			PC = 0;
			out();

			haltFlag = false;
			while(!haltFlag)
			{
				busPC();
				loadMAR();
				busRAM();
				loadC();
				out();
				if ((regc & 0b11110000) == 0b0)
				{
					LDA();
				}
				else if (((regc & 0b11110000) >> 4)== 0b1)
				{
					ADD();
				}
				else if (((regc & 0b11110000) >> 4) == 0b10)
				{
					SUB();
				}
				else if (((regc & 0b11110000) >> 4) == 0b1110)
				{
					OUTPUT();
				}
				else if (((regc & 0b11110000) >> 4) == 0b1111)
				{
					HLT();
				}
				sleep(1000 / clockrate);
			}
			state = "Halted";
			out();
		}
		else
		{
			if (codeFile == "")
				throwError("No file loaded...");
			else
				throwError("Error loading file...");
		}
	}

	void out()
	{
		system("clear");
		outDisp();
		std::cout << "\nReg A:\n";
		outRegA();
		std::cout << "ALU:\n";
		outALU();
		std::cout << "Reg B:\n";
		outRegB();
		std::cout << "Reg C:\n";
		outRegC();
		std::cout << "BUS:\n";
		outWBus();
		std::cout << "PC:\n";
		outPC();
		std::cout << "RAM:\n";
		outRAM();
		std::cout << "\nState: ";
		outState();
		std::cout << "Mnemonic: ";
		outMnemonic();
	}

	void outRegA()
	{
		if (bin)
			std::cout << std::bitset<8>(static_cast<uint16_t>(rega)) << '\n';
		else
			std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(rega) << '\n';
	}

	void outRegB()
	{
		if (bin)
			std::cout << std::bitset<8>(static_cast<uint16_t>(regb)) << '\n';
		else
			std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(regb) << '\n';
	}

	void outRegC()
	{
		if (bin)
			std::cout << std::bitset<8>(static_cast<uint16_t>(regc)) << '\n';
		else
			std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(regc) << '\n';
	}

	void outWBus()
	{
		if (bin)
			std::cout << std::bitset<8>(static_cast<uint16_t>(wbus)) << '\n';
		else
			std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(wbus) << '\n';
	}

	void outALU()
	{
		if (bin)
			std::cout << std::bitset<8>(static_cast<uint16_t>(ALU)) << '\n';
		else
			std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(ALU) << '\n';
	}

	void outRAM()
	{
		for (int addr = 0; addr < 16; addr++)
		{
			if (bin)
			{
				std::cout << std::bitset<8>(static_cast<uint16_t>(RAM[addr])) << '\n';
				if (addr % 4 == 3)
					std::cout << '\n';
			}
			else 
			{
				std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(RAM[addr]) << ' ';
				if (addr % 4 == 3)
					std::cout << '\n';
			}
		}
	}

	void outState()
	{
		std::cout << state << '\n';
	}
	void outMnemonic()
	{
		std::cout << op << '\n';
	}

	void outPC()
	{
		if (bin)
			std::cout << std::bitset<4>(static_cast<uint16_t>(PC)) << '\n';
		else
			std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(PC) << '\n';
	}

	void outDisp()
	{
		ssdisp dsp(DISP);
		dsp.out();
	}

};


void throwError(std::string e)
{
	//SetConsoleTextAttribute(hConsole, 12);
	std::cout << e << "\n";
	//SetConsoleTextAttribute(hConsole, 15);
}