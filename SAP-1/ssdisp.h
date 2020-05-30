#pragma once
#include <cstdint>
#include <string>

class ssdisp
{
public:
	uint32_t num = 0;
	ssdisp() {}
	ssdisp(uint32_t _num) : num(_num) {}

	void out()
	{
		std::string snum = std::to_string(num);
		const char* s = snum.c_str();
		int i, j;
		const char* c = "|_|";
		unsigned int d[9] = { 0, 6947821, 0, 7209841, 7734140, 1180575, 8257861, 3933037, 1442811 };
		for (i = 0; i < 9; i += 3) 
		{
			for (const char* p = s; *p; ++p)
			{
				for (j = 0; j != 3; ++j)
					std::cout << (1 << ((*p > 96 ? *p - 32 : *p) - '0') & d[j + i] ? c[j] : ' ');
				std::cout << (' ');
			}
			std::cout << ('\n');
		}
	}

};