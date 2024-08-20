#pragma once
#include <stdio.h>
#include "Core.h"
#include "GraphicsDeviceManager.hpp"



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

	//Imgui manage functions.

	void SetImguiAPI(const rhi::GraphicsAPI& api) const;
	
	inline GraphicsDeviceManager& GetWindow() { return *m_Window; }
	inline static App& GetApp()				  { return *s_Instance; }
	


protected:
	void SetEventBus(GLFWEventSource* eventBus) { m_EventBus = eventBus; }
private:

	using ImGuiRenderFunction = std::function<void()>;

	std::unordered_map<rhi::GraphicsAPI, ImGuiRenderFunction> m_ImguiRenderFunctionMap;

	std::unique_ptr<GraphicsDeviceManager> m_Window;
	std::unique_ptr<GLFWEventListener> m_GLFWEventListener;
	std::unique_ptr<EventCameraListener> m_CameraEventListener;
	LayerStack m_LayerStack;
	GLFWEventSource* m_EventBus = nullptr;
	rhi::GraphicsAPI m_API;

   static App* s_Instance;

private:
	//TODO : Implement this function in app.tpp. this is only sugar sintax to reduce time to use. 
	template<typename EvenType>
	void ConnectEvent() const;
	

	void InitiateEventBusLayers();
	void RenderImGuiFrame() const;
	bool isRunning() const;

	float m_Time = 0;
};

App* CreateApp();

IFNITY_END_NAMESPACE

#include "App.cxx"

