#include "pch.h"

#include "App.h"
#include "Ifnity/Layers/ExampleLayer.h" //TODO: BORRAR , your own cpp 
#include "Ifnity/Layers/NVML_Layer.hpp"
#include "Platform/ImguiRender/ImguiOpenglRender.h"
#include "Platform/ImguiRender/ImguiD3D11Render.h"
#include "Platform/ImguiRender/ImguiD3D12Render.h"
#include <GLFW/glfw3.h>
#include <glad\glad.h>
#include <Platform/Windows/DeviceOpengl.h>
#include <Platform/Windows/DeviceD3D11.h>



#define _MODO_TEST 1



namespace IFNITY {





	static const char* shaderCodeVertex = R"(

#version 450

const vec2 _31[3] = vec2[](vec2(-0.5), vec2(0.0, 0.5), vec2(0.5, -0.5));
const vec3 _35[3] = vec3[](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));

layout(location = 0) out vec3 out_var_COLOR;

void main()
{
    gl_Position = vec4(_31[uint(gl_VertexID)], 0.0, 1.0);
    out_var_COLOR = _35[uint(gl_VertexID)];
}




)";

	static const char* shaderCodeFragment = R"(
#version 450


layout(location = 0) in vec3 in_var_COLOR;
layout(location = 0) out vec4 out_var_SV_Target0;

void main()
{
    out_var_SV_Target0 = vec4(in_var_COLOR, 1.0);
}




)";

	void App::InitApp(rhi::GraphicsAPI api)
	{
		// Create windows props
		WindowData props;

		m_Window = std::unique_ptr<GraphicsDeviceManager>(
			GraphicsDeviceManager::Create(api));

		m_Window->CreateWindowSurface(std::move(props));


		InitEventBusAndListeners();

#ifndef _MODO_TEST
		InitConfigurationImGui();
#endif

	}

	void App::InitEventBusAndListeners()
	{
		// Intialize the EventListenerControler
		
			m_GLFWEventListener = std::make_unique<GLFWEventListener>();
		
			SetEventBus(m_Window->GetGLFWEventSource());

			// Connect Differents events. 
			CONNECT_EVENT(WindowResize);
			CONNECT_EVENT(WindowClose);
			CONNECT_EVENT(KeyPressed);
			CONNECT_EVENT(KeyRelease);
			CONNECT_EVENT(MouseMove);
			CONNECT_EVENT(ScrollMouseMove);
			CONNECT_EVENT(MouseClick);

	}
		//Example the simple event connect. 

		//events::connect<MouseMove>(*m_Window->GetGLFWEventSource(), *m_CameraEventListener);
	

	// Static member  declaration
	App* App::s_Instance = nullptr;
	// Default Constructor;
	App::App(rhi::GraphicsAPI api) : m_graphicsAPI(api)
	{
		s_Instance = this;

		InitApp(m_graphicsAPI);
		
	}

	void App::InitConfigurationImGui()
	{

		// Initialize ImGui and set m_graphicsAPI; 
		InitializeImGui();
		m_graphicsAPI = GraphicsDeviceManager::GetStaticGraphicsAPI();

		SetImguiAPI();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiBackendFlags_HasMouseCursors; // Enable SetMousePos.
		io.ConfigFlags |= ImGuiBackendFlags_HasSetMousePos;  // Enable SetMousePos.
		io.FontGlobalScale = 1.0f;
		//ImGui::StyleInfity(); // Clasic color style.

		// Classic version  1.87 see IMGUI_DISABLE_OBSOLETE_KEYIO in new version
		//  not necessary intialization maps for keys.
		// io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		//TODO: Move this in Gharpics Device Manager. 
		m_ImguiRenderFunctionMap[rhi::GraphicsAPI::OPENGL] = []()
			{
				ImGui_ImplOpenGL3_NewFrame();
				ImGui::NewFrame();
				
			};
		m_ImguiRenderFunctionMap[rhi::GraphicsAPI::D3D11] = []()
			{
				ImGui_ImplDX11_NewFrame();
				ImGui::NewFrame();
				

			};
		m_ImguiRenderFunctionMap[rhi::GraphicsAPI::D3D12] = []()
			{
			
				ImGui_ImplDX12_NewFrame();
				ImGui::NewFrame();
				
			
			};
		m_ImguiRenderFunctionMap[rhi::GraphicsAPI::VULKAN] = []() {};


	}
	App::~App()
	{

		// OnDetach all layers
		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
		}
		s_Instance = nullptr;
		IFNITY_LOG(LogApp, INFO, "App is destroyed");
	}

	void App::run()
	{
		//This part its because the initializacion process is in the constructor of the app, then source will be build LAYERS  after App constructor. We cant initiate EventBusLayers in App constructor. 
		InitiateEventBusLayers();

#ifndef _MODO_TEST 
		// TODO: CHange this logic, now is usefull to debug  this should be in a layer. 
		bool init = false;
		if (m_graphicsAPI == rhi::GraphicsAPI::OPENGL && !init)
		{
			DeviceOpengl::DemoTriangle(shaderCodeVertex, shaderCodeFragment);

		}
#endif
		while (isRunning())
		{
			glfwPollEvents();
			m_Window->RenderDemo(m_Window->GetWidth(), m_Window->GetHeight());

			// Render ImGui Frame
#ifndef _MODO_TEST
			RenderImGuiFrame();
#endif
			//ImGui::ShowDemoWindow();
			//Layer Renders. 
			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
			}


			m_Window->OnUpdate();
#ifndef _MODO_TEST

			// Change API 
			if (m_FlagChangeAPI)
			{
				//Delete and destroy windows. 
				m_Window->Shutdown();
				

				//OnDetach all layers
				ForceOnDetachLayers();
				m_Window.reset();

				ResetAppEvents();

				InitApp(m_graphicsAPI);
				
				ForceOnAttachLayers();
				InitiateEventBusLayers();
				m_FlagChangeAPI = false;

				if (m_graphicsAPI == rhi::GraphicsAPI::OPENGL && !init)
				{
					DeviceOpengl::DemoTriangle(shaderCodeVertex, shaderCodeFragment);

				}
				

			}
#endif
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
		for (Layer* layer : m_LayerStack)
		{
			layer->ConnectToEventBus(m_EventBus);
		}
	}

	void App::ForceOnAttachLayers() 
	{
		for (Layer* layer : m_LayerStack)
		{
			layer->OnAttach();
		}
	}

	void App::ForceOnDetachLayers()
	{
		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
		}
	}

	//The app is running until the user close the window or the flag change API is true.
	bool App::isRunning() const { return m_GLFWEventListener->getRunning(); }

	void App::SetImguiAPI() const
	{
		// If you use other different lib, api or framework to generate a window , you should change this.
		m_Window->InitImGui();

	}

	void App::RenderImGuiFrame() const
	{
		ImGuiIO& io = ImGui::GetIO();
		App& app = App::GetApp();

		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
		/*IFNITY_LOG(LogApp, INFO,
			"Width imgui : " + std::to_string(app.GetWindow().GetWidth()) +
			" Height imgui : " + std::to_string(app.GetWindow().GetHeight()));*/

		float time = (float)glfwGetTime();

		io.DeltaTime = m_Time > 0.0 ? (float)(time - m_Time) : (float)(1.0f / 60.0f);

		// Render ImguiFrame
		auto it = m_ImguiRenderFunctionMap.find(m_graphicsAPI);

		//Todo change this because its not optimal , better a function pointer and setting and the initialize .
		if (it != m_ImguiRenderFunctionMap.end())
		{
			it->second();
		}
		else
		{
			IFNITY_LOG(LogApp, ERROR, "Imgui API not found, impossible to render");
		}


	}

	void App::ResetAppEvents()
	{
		m_GLFWEventListener.reset();
		m_EventBus = nullptr;

	}
} // namespace IFNITY
