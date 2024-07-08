#include "pch.h"

#include "App.h"
#include "Log.h"
#include "Event/WindowEvent.h"
#include "Window.h"

namespace IFNITY
{
	#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	// Reload Function 


	//Default Constructor;
	App::App()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(App::OnEvent));
	}
	App::~App()
	{
		printf("~App() \n");
	}


	void App::run()
	{


		while(m_runing)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}

	}

	void App::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		//This will call the function OnWindowClose if the event is WindowCloseEvent type do this function. 
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(App::OnWindowClose));
		IFNITY_LOG(LogCore,TRACE, e.ToString());
	}

	bool App::OnWindowClose(WindowCloseEvent& e)
	{
		m_runing = false;
		return true;
	}

}
