#pragma once

#include "pch.h"
#include "Ifnity/Core.h"
#include "Ifnity/Event/EventMacro.h"

namespace IFNITY {

	EVENT2(WindowResize, int, Width, int, Height);
	EVENT(WindowClose,"Window Close Event");



	#define WINDOW_EVENT_GROUP       WindowResize, \
								     WindowClose 

	
	

}