#pragma once
#include <stdio.h>
#include "Core.h"
#include "Window.h"

#include "LayerStack.hpp"

IFNITY_NAMESPACE
	
//forward declaration
	class WindowCloseEvent;
	class IFNITY_API App
	{
	public:
		App();
		virtual ~App();
		void run();



		//Layer manage functions.
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);


	protected:
		void SetEventBus(GLFWEventSource* eventBus) { m_EventBus = eventBus; }
	private:
		
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<GLFWEventListener> m_GLFWEventListener;
		std::unique_ptr<EventCameraListener> m_CameraEventListener;

		LayerStack m_LayerStack;
		GLFWEventSource* m_EventBus = nullptr;

		//TODO : Implement this function in app.tpp. this is only sugar sintax to reduce time to use. 
		template<typename EvenType>
		void ConnectEvent() const
		{
			events::connect<EvenType>(*m_Window->GetGLFWEventSource(), *m_GLFWEventListener);
		}

		void InitiateEventBusLayers();
		
		bool isRunning() const;
	};

	App* CreateApp();

IFNITY_END_NAMESPACE


