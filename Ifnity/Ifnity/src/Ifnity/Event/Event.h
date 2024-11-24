#pragma once

#include "pch.h"

#include "Ifnity/Event/EventMacro.h"

namespace IFNITY {
    
    // Event Creating . 
	EVENT3(MouseClick, int, Button, int, State, int, Mods)
	EVENT2(MouseMove, int, X, int, Y);
    EVENT2(WindowResize, int, Width, int, Height);
    EVENT3(KeyPressed, int, Key, int, Action,int, Mode);
    EVENT2(ScrollMouseMove, int, X, int, Y);
	EVENT1(KeyRelease, int, Key)
    EVENT(WindowClose,"Window Close Event");


    #define WINDOW_EVENT_GROUP       WindowResize, \
                                     WindowClose , \
                                     KeyPressed, \
									 KeyRelease, \
									 MouseMove, \
									 ScrollMouseMove, \
									 MouseClick

	#define CAMERA_EVENT_GROUP       MouseMove,\
									 MouseClick,\
									 KeyPressed
									 


}