

#include "ImguiLayer.h"
#include "Ifnity/Event/WindowEvent.h"
#include "imgui.h"
#include "ImPlot.h"
#include "Platform/ImguiRender/ImguiOpenglRender.h"
#include <GLFW\glfw3.h>

#include "../App.h"


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
	m_monitor.setDisplay(&m_NvmlDisplayMonitor);

}

void ImguiLayer::OnDetach()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

void ImguiLayer::OnUpdate()
{
 //   m_monitor.refresh();
	//ImGuiIO& io = ImGui::GetIO();
	//App& app = App::GetApp();
	//
	//io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
	//IFNITY_LOG(LogApp, INFO, "Width: " + std::to_string(app.GetWindow().GetWidth()) + " Height: " + std::to_string(app.GetWindow().GetHeight()));

	//float time = (float)glfwGetTime();

	//io.DeltaTime = m_Time > 0.0 ? (float)(time - m_Time) : (float)(1.0f / 60.0f);
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui::NewFrame();
	//ImPlot::CreateContext();
	//ImPlot::ShowDemoWindow();
	//ImGui::Text("Hello, world %d", 123);
	//if(ImGui::Button("Save"))
	//static bool show = true;
	////Mostrar numeros aleatorios en imgui en texto 
	//
	//		//Crear aleatorio 
	//int random = rand() % 100 + 1;
	//ImGui::Text("Random number: %d", random);
	//m_monitor.display();
	////ImGui::ShowDebugLogWindow();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void ImguiLayer::onEventReceived(const WindowResize& event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)event.getWidth(), (float)event.getHeight());
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	glViewport(0, 0, event.getWidth(), event.getHeight());


}
void ImguiLayer::onEventReceived(const KeyPressed& event)
{
	//Key pressed imgui 
	auto& io = ImGui::GetIO();

	io.AddInputCharacter(event.getKey());

}

void ImguiLayer::onEventReceived(const KeyRelease & event)
{}

void ImguiLayer::onEventReceived(const MouseMove & event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent((float)event.getX(), (float)event.getY());

}

void ImguiLayer::onEventReceived(const ScrollMouseMove & event)
{
		ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent((float)event.getX(), (float)event.getY());



}

void ImguiLayer::onEventReceived(const MouseClick & event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(event.getButton(), event.getState());


}


IFNITY_END_NAMESPACE
