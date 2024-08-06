

#include "ImguiLayer.h"
#include "Ifnity/Event/WindowEvent.h"
#include "imgui.h"
#include "Platform/ImguiRender/ImguiOpenglRender.h"
#include <GLFW\glfw3.h>



IFNITY_NAMESPACE
ImguiLayer::ImguiLayer() : Layer("ImguiLayer")
{
}


ImguiLayer::~ImguiLayer()
{
}

void ImguiLayer::ConnectToEventBusImpl(void* bus)
{
	auto eventBus = static_cast<IFNITY::GLFWEventSource*>(bus);
	if (eventBus)
	{
		CONNECT_EVENT_LAYER(WindowResize, eventBus);
		CONNECT_EVENT_LAYER(WindowClose, eventBus);
		CONNECT_EVENT_LAYER(KeyPressed, eventBus);
		CONNECT_EVENT_LAYER(KeyRelease, eventBus);
		CONNECT_EVENT_LAYER(MouseMove, eventBus);
		CONNECT_EVENT_LAYER(ScrollMouseMove, eventBus);
		CONNECT_EVENT_LAYER(MouseClick, eventBus);
	}
	else
	{
		IFNITY_LOG(LogApp, ERROR, "The pointer is not type IFNITY::GLFWEventSource*");
	}

	
	
}

void ImguiLayer::OnAttach()
{
	// Initialize ImGui
	ImGui::CreateContext();
	

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsClassic();							// Clasic color style. 

	//Classic version  1.87 see IMGUI_DISABLE_OBSOLETE_KEYIO in new version 
	// not necessary intialization maps for keys. 
	//io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;					

	ImGui_ImplOpenGL3_Init("#version 450");
	
}

void ImguiLayer::OnDetach()
{
}

void ImguiLayer::OnUpdate()
{
    
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)1280, (float)720);

	float time = (float)glfwGetTime();

	io.DeltaTime = m_Time > 0.0 ? (float)(time - m_Time) : (float)(1.0f / 60.0f);

	


	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	static bool show = true;
	ImGui::ShowDemoWindow(&show);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImguiLayer::onEventReceived(const KeyPressed& event)
{}

void ImguiLayer::onEventReceived(const KeyRelease & event)
{}

void ImguiLayer::onEventReceived(const MouseMove & event)
{}

void ImguiLayer::onEventReceived(const ScrollMouseMove & event)
{}

void ImguiLayer::onEventReceived(const MouseClick & event)
{}


IFNITY_END_NAMESPACE
