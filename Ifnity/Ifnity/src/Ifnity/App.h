#pragma once
#include <stdio.h>
#include "GraphicsDeviceManager.hpp"



#include "LayerStack.hpp"
#include "ImGuiContextManager.h"
#include "Graphics/Interfaces/IShader.hpp"



IFNITY_NAMESPACE

//forward declaration
class WindowCloseEvent;
class IFNITY_API App
{
public:
	App(rhi::GraphicsAPI api);
	virtual ~App();

	void InitApp(rhi::GraphicsAPI api);
	void run();


	//----------------------------------------------------------------------------------------------//
	//INTEFACE FUNCTIONS
	//----------------------------------------------------------------------------------------------//
	virtual void Initialize() = 0;
	virtual void Render() = 0;
	virtual void Animate() = 0;


	//Layer manage functions.
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	//Imgui manage functions.

	void SetImguiAPI() const;

	inline GraphicsDeviceManager& GetManagerDevice() { return *m_ManagerDevice; }
	inline const GraphicsDeviceManager& GetManagerDevice() const { return *m_ManagerDevice; }
	inline GraphicsDeviceManager* GetDevicePtr() { return m_ManagerDevice.get(); }

	inline static App& GetApp() { return *s_Instance; }
	inline rhi::GraphicsAPI GetGraphicsAPI() const { return m_graphicsAPI; }
	inline void SetGraphicsAPI(rhi::GraphicsAPI api, bool flagChange = false)
	{

		m_graphicsAPI = api;
		m_FlagChangeAPI = flagChange;
	}
	inline float GetTime() const { return (float)glfwGetTime(); }


protected:
	void SetEventBus(GLFWEventSource* eventBus) { m_EventBus = eventBus; }




private:
	using ImGuiRenderFunction = std::function<void()>;
	std::unordered_map<rhi::GraphicsAPI, ImGuiRenderFunction> m_ImguiRenderFunctionMap;

	std::unique_ptr<GraphicsDeviceManager> m_ManagerDevice;
	std::unique_ptr<GLFWEventListener> m_GLFWEventListener;
	std::unique_ptr<EventCameraListener> m_CameraEventListener;
	LayerStack m_LayerStack;
	GLFWEventSource* m_EventBus = nullptr;
	rhi::GraphicsAPI m_API;


	bool m_FlagChangeAPI = false;


	static App* s_Instance;

	//IShaders 
	IShader* m_VS = nullptr;
	IShader* m_PS = nullptr;


private:
	//TODO : Implement this function in app.tpp. this is only sugar sintax to reduce time to use. 
	template<typename EvenType>
	void ConnectEvent() const;
	void InitConfigurationImGui();
	void InitEventBusAndListeners();
	void InitiateEventBusLayers();
	void RenderImGuiFrame() const;
	bool isRunning() const;
	void ForceOnAttachLayers() ;
	void ForceOnDetachLayers();
	void ResetAppEvents();
	float m_Time = 0;
	rhi::GraphicsAPI m_graphicsAPI;
};

App* CreateApp();

IFNITY_END_NAMESPACE

#include "App.cxx"

