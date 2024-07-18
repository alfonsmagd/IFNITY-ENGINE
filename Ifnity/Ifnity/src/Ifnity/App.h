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
		bool isRunning();
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<GLFWEventListener> m_GLFWEventListener;

		//TODO : Implement this function in app.tpp.
		template<typename EvenType>
		void ConnectEvent()
		{
			events::connect<EvenType>(*m_Window->GetGLFWEventSource(), *m_GLFWEventListener);
		}
		
		
	};

	App* CreateApp();

IFNITY_END_NAMESPACE


