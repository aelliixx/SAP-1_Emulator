#include <iostream>
#include "SAPcpu.h"


int main()
{
	std::cout << "SAP-1 emulator:\n";

	SAPcpu A("sampleCode.sap");

	std::string cmd;

	while (true)
	{
		std::cout << "\n> ";
		std::getline(std::cin, cmd);
		if (cmd.substr(0, 3) == "RUN")
		{
			A.run();
		}
		else if (cmd.substr(0, 4) == "LOAD")
		{
			if (cmd.size() > 5)
				A.openFile(cmd.substr(5, cmd.size()));
			else
				throwError("ERROR: Expected argument: LOAD _");
			//std::cout << cmd.substr(5, cmd.size()) << '\n';
		}
		else if (cmd.substr(0, 5) == "CLOSE")
		{
			A.closeFile();
		}
		else if (cmd == "")
		{
			continue;
		}
		else if (cmd == "QUIT")
		{
			break;
		}
		else if (cmd == "HELP")
		{
			std::cout << "List of available commands:\nRUN - runs the current loaded file.\nLOAD _ - takes a filename or filepath argument and loads the given file into memory.\nCLOSE - closes any opened files.\nBIN - outputs CPU values in binary.\nHEX - outputs CPU values as hex.\nQUIT - exits program.\n";
		}
		else if (cmd == "BIN")
		{
			A.bin = true;
		}
		else if (cmd == "HEX")
		{
			A.bin = false;
		}
		else
			throwError("Uknown command...");
	}


	return 0;
}