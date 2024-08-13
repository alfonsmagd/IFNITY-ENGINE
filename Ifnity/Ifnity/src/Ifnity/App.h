#pragma once
#include <stdio.h>
#include "Core.h"
#include "GraphicsDeviceManager.h"



#include "LayerStack.hpp"
#include "ImGuiContextManager.h"
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

	inline GraphicsDeviceManager& GetWindow() { return *m_Window; }
	inline static App& GetApp()				  { return *s_Instance; }

protected:
	void SetEventBus(GLFWEventSource* eventBus) { m_EventBus = eventBus; }
private:

	std::unique_ptr<GraphicsDeviceManager> m_Window;
	std::unique_ptr<GLFWEventListener> m_GLFWEventListener;
	std::unique_ptr<EventCameraListener> m_CameraEventListener;
	LayerStack m_LayerStack;
	GLFWEventSource* m_EventBus = nullptr;

   static App* s_Instance;

private:
	//TODO : Implement this function in app.tpp. this is only sugar sintax to reduce time to use. 
	template<typename EvenType>
	void ConnectEvent() const
	{
		events::connect<EvenType>(*m_Window->GetGLFWEventSource(), *m_GLFWEventListener);
	}

	void InitiateEventBusLayers();

	bool isRunning() const;

	float m_Time = 0;
};

App* CreateApp();

IFNITY_END_NAMESPACE


