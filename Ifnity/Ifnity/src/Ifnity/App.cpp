#include "pch.h"

#include "App.h"
#include "GraphicsDeviceManager.h"
#include <glad\glad.h>
#include <GLFW/glfw3.h>




namespace IFNITY
{
	//#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	// Reload Function 

	#define CONNECT_EVENT(x) ConnectEvent<x>()

	//Default Constructor;
	App::App()
	{

		m_Window = std::unique_ptr<GraphicsDeviceManager>(GraphicsDeviceManager::Create());

		m_Window->CreateWindowSurface(WindowProps());
		//Intialize the EventListenerControler 
		m_GLFWEventListener = std::make_unique<GLFWEventListener>();
		
		
		
		SetEventBus(m_Window->GetGLFWEventSource());

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
		InitiateEventBusLayers();

		

		while(isRunning())
		{
		
			glClearColor(1, 0.6, 0.6, 0.3);
			glClear(GL_COLOR_BUFFER_BIT);

			for(Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
			}

			
			m_Window->OnUpdate();
		}
		

		m_Window->Shutdown();
	}

	void App::PushLayer(Layer* layer)
	{
		
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();

	
	}

	void App::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();

	}

	void App::InitiateEventBusLayers()
	{
		for(Layer* layer : m_LayerStack)
		{
			layer->ConnectToEventBus(m_EventBus);
		}
	
	}

	bool App::isRunning() const
	{
		return m_GLFWEventListener->getRunning();
	}


}
