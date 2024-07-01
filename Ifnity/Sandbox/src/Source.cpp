

// IFNITY.cp
#include "pch.h"
#include <Ifnity.h>


IFNITY_NAMESPACE



	class Source :  public IFNITY::App
	{
	public:
		Source() = default;
		~Source() {  }
	};

	

	IFNITY::App* IFNITY::CreateApp()
	{

		return new Source();
	}

IFNITY_END_NAMESPACE
