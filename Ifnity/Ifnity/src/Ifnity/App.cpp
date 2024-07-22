#include "pch.h"

#include "App.h"
#include "Log.h"
#include "Window.h"



namespace IFNITY
{
	//#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	// Reload Function 

	#define CONNECT_EVENT(x) ConnectEvent<x>()

	//Default Constructor;
	App::App()
	{

		m_Window = std::unique_ptr<Window>(Window::Create());
		//Intialize the EventListenerControler 
		m_GLFWEventListener = std::make_unique<GLFWEventListener>();
		m_CameraEventListener = std::make_unique<EventCameraListener>();


		CONNECT_EVENT(WindowResize);
		CONNECT_EVENT(WindowClose);
		CONNECT_EVENT(KeyPressed);
		CONNECT_EVENT(KeyRelease);
		CONNECT_EVENT(MouseMove);
		CONNECT_EVENT(ScrollMouseMove);
		CONNECT_EVENT(MouseClick);	

		

	}
	App::~App()
	{
		printf("~App() \n");
	}


	void App::run()
	{

		while(isRunning())
		{
			glClearColor(0, 0.6, 0.6, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
		
	}

	bool App::isRunning()
	{
		return m_GLFWEventListener->getRunning();
	}


}
