

// IFNITY.cp
#include "pch.h"
#include <Ifnity.h>


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

