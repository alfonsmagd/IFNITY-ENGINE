#include "pch.h"

#include "App.h"
#include "Log.h"
#include "Window.h"



namespace IFNITY
{
	//#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	// Reload Function 


	//Default Constructor;
	App::App()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());

		//Intialize the EventListenerControler 
		m_GLFWEventListener = std::make_unique<GLFWEventListener>();
		//events::connect<WindowResizeEvent>(m_GLFWEventSource, m_GLFWEventListener);
		//Connect with the EventSourceController 
		
		events::connect<WindowResize>(*m_Window->GetGLFWEventSource(), *m_GLFWEventListener);
		events::connect<WindowClose>(*m_Window->GetGLFWEventSource(),  *m_GLFWEventListener);

	}
	App::~App()
	{
		printf("~App() \n");
	}


	void App::run()
	{

		while(m_GLFWEventListener.get()->getRunning())
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
		
	}


}
