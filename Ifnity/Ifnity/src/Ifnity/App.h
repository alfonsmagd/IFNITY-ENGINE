#pragma once
#include <stdio.h>
#include "Core.h"
#include "Window.h"
#include "Event/WindowEvent.h"

IFNITY_NAMESPACE
	
//forward declaration
	class WindowCloseEvent;
	class IFNITY_API App
	{
	public:
		App();
		virtual ~App();
		void run();

	private:
		bool m_runing = true;
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<GLFWEventListener> m_GLFWEventListener;

		
	};

	App* CreateApp();

IFNITY_END_NAMESPACE

