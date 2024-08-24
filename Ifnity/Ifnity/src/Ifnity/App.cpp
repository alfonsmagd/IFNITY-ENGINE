#include "pch.h"

#include "App.h"
#include "GraphicsDeviceManager.hpp"
#include "Ifnity/Layers/ExampleLayer.h" //TODO: BORRAR , your own cpp 
#include "Ifnity/Layers/NVML_Layer.hpp"
#include "Platform/ImguiRender/ImguiOpenglRender.h"
#include "Platform/ImguiRender/ImguiD3D11Render.h"
#include <GLFW/glfw3.h>
#include <glad\glad.h>
#include <Platform/Windows/DeviceOpengl.h>
#include <Platform/Windows/DeviceD3D11.h>


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

	// Static member  declaration
	App* App::s_Instance = nullptr;
	// Default Constructor;
	App::App()
	{
		s_Instance = this;
		// Create windows props
		WindowData props;
	
		m_Window = std::unique_ptr<GraphicsDeviceManager>(
			GraphicsDeviceManager::Create(rhi::GraphicsAPI::D3D11));

		m_Window->CreateWindowSurface(props);
		// Intialize the EventListenerControler
		m_GLFWEventListener = std::make_unique<GLFWEventListener>();

		SetEventBus(m_Window->GetGLFWEventSource());

		//events::connect<MouseMove>(*m_Window->GetGLFWEventSource(), *m_CameraEventListener);

		CONNECT_EVENT(WindowResize);
		CONNECT_EVENT(WindowClose);
		CONNECT_EVENT(KeyPressed);
		CONNECT_EVENT(KeyRelease);
		CONNECT_EVENT(MouseMove);
		CONNECT_EVENT(ScrollMouseMove);
		CONNECT_EVENT(MouseClick);

		// Initialize ImGui and set m_graphicsAPI; 
		InitializeImGui();
		m_graphicsAPI = GraphicsDeviceManager::GetStaticGraphicsAPI();

		SetImguiAPI();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiBackendFlags_HasMouseCursors; // Enable SetMousePos.
		io.ConfigFlags |= ImGuiBackendFlags_HasSetMousePos;  // Enable SetMousePos.
		io.FontGlobalScale = 1.0f;
		ImGui::StyleInfity(); // Clasic color style.

		// Classic version  1.87 see IMGUI_DISABLE_OBSOLETE_KEYIO in new version
		//  not necessary intialization maps for keys.
		// io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		//TODO: Move this in Gharpics Device Manager. 
		m_ImguiRenderFunctionMap[rhi::GraphicsAPI::OPENGL] = []()
			{
				ImGui_ImplOpenGL3_NewFrame();
				ImGui::NewFrame();
				ImPlot::CreateContext();
			};
		m_ImguiRenderFunctionMap[rhi::GraphicsAPI::D3D11] = []()
			{
				ImGui_ImplDX11_NewFrame();
				ImGui::NewFrame();
				ImPlot::CreateContext();
				
			};
		m_ImguiRenderFunctionMap[rhi::GraphicsAPI::D3D12] = []() {};
		m_ImguiRenderFunctionMap[rhi::GraphicsAPI::VULKAN] = []() {};
	}
	App::~App()
	{

		// OnDetach all layers
		for ( Layer* layer : m_LayerStack )
		{
			layer->OnDetach();
		}
		s_Instance = nullptr;
		IFNITY_LOG(LogApp, INFO, "App is destroyed");
	}

	void App::run()
	{
		InitiateEventBusLayers();

		// TODO: CHange this logic, now is usefull to debug  this should be in a layer. 
		if ( m_graphicsAPI == rhi::GraphicsAPI::OPENGL )
		{
			DeviceOpengl::DemoTriangle(shaderCodeVertex, shaderCodeFragment);

		}
		while ( isRunning() )
		{
			//if use D3D11 resize the swapchain this is in other place. 
			/*auto d3d11Manager = dynamic_cast<DeviceD3D11*>(m_Window.get());
			if ( d3d11Manager )
			{
				d3d11Manager->ResizeSwapChain();
			}*/

			m_Window->RenderDemo(m_Window->GetWidth(), m_Window->GetHeight());

			
			// Render ImGui Frame
			RenderImGuiFrame();
			ImGui::ShowDemoWindow();
			//Layer Renders. 
			for ( Layer* layer : m_LayerStack )
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
		for ( Layer* layer : m_LayerStack )
		{
			layer->ConnectToEventBus(m_EventBus);
		}
	}

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
		IFNITY_LOG(LogApp, INFO,
			"Width imgui : " + std::to_string(app.GetWindow().GetWidth()) +
			" Height imgui : " + std::to_string(app.GetWindow().GetHeight()));

		float time = (float)glfwGetTime();

		io.DeltaTime = m_Time > 0.0 ? (float)(time - m_Time) : (float)(1.0f / 60.0f);

		// Render ImguiFrame
		auto it = m_ImguiRenderFunctionMap.find(m_graphicsAPI);

		//Todo change this because its not optimal , better a function pointer and setting and the initialize .
		if ( it != m_ImguiRenderFunctionMap.end() )
		{
			it->second();
		}
		else
		{
			IFNITY_LOG(LogApp, ERROR, "Imgui API not found, impossible to render");
		}
	}


} // namespace IFNITY
