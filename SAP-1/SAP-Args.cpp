#include <iostream>
#include "SAPcpu.h"
#include <string>


int flag(std::string flag, char* args[], int argl)
{
	for (int i = 0; i < argl; i++)
	{
		if (args[i] == flag)
			return i;
	}
	return -1;
}

bool hasEnding (std::string const &fullString, std::string const &ending)
{
	if (fullString.length() >= ending.length()) 
		return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
	else 
		return false;
}

int main(int argc, char *argv[])
{
	SAPcpu A;

	int flg = flag("-x", argv, argc);
	if (flg!=-1)
	{
		std::string file = argv[flg+1];
		if (!hasEnding(file, ".sap"))
		{
			std::cout << "Invalid file provided: " << file;
			return 0;
		}

		std::cout << "SAP-1 emulator:\n";

		flg = flag("-c", argv, argc);
		if (flg!=-1 && argv[flg+1] > 0)
		{
			A.setClockrate(atoi(argv[flg+1]));
		}
		flg = flag("-b2", argv, argc);
		if (flg!=-1)
		{
			A.bin = true;
			goto RUN;
		}
		flg = flag("-b16", argv, argc);
		if (flg!=-1)
		{
			A.bin = false;
			goto RUN;
		}
		else if (flg=-1)
			std::cout << "Invalid base. Quitting.\n";

		RUN:

		A.openFile(file);
			A.run();
	}

	flg = flag("-h", argv, argc);

	if (flg!=-1)
		std::cout << "List of available commands:\n-x [FILE NAME] -- loads and executes the given file.\n-b2 / -b16 -- sets registers to be printed in either base 2 or 16 respectively.\n-h -- shows this page.\n";
	
	return 0;
}