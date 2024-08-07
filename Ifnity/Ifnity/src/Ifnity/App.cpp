#include "pch.h"

#include "App.h"
#include "GraphicsDeviceManager.h"
#include <glad\glad.h>
#include <GLFW/glfw3.h>




namespace IFNITY
{

	//Static member  declaration
	App* App::s_Instance = nullptr;
	//Default Constructor;
	App::App()
	{
		s_Instance = this;

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
		
		//OnDetach all layers
		for(Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
		}
		s_Instance = nullptr;
		IFNITY_LOG(LogApp, INFO, "App is destroyed");

	}


	void App::run()
	{
		InitiateEventBusLayers();

		

		while(isRunning())
		{
		
			glClearColor(0.0, 0.1, 0.1, 0.3);
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
