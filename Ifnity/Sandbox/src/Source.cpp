

// IFNITY.cp
#include "pch.h"
#include <Ifnity.h>

void printf(const char* str)
{
	std::cout << str << std::endl;
};

class Source: public IFNITY::App
{
public:
	Source() = default;
	~Source() {}
};



IFNITY::App* IFNITY::CreateApp()
{

	return new Source();
}

