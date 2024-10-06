

#include "ImguiLayer.h"
#include "Ifnity/Event/WindowEvent.h"
#include "imgui.h"
#include "ImPlot.h"
#include "Platform/ImguiRender/ImguiOpenglRender.h"
#include "Platform/ImguiRender/ImguiD3D11Render.h"
#include "Platform/ImguiRender/ImguiD3D12Render.h"
#include "Platform/ImguiRender/ImguiVulkanRender.h"
#include <GLFW\glfw3.h>

#include "../App.h"




IFNITY_NAMESPACE

//Api Function 

rhi::ImGuiRenderDrawDataFn       ImGuiRenderDrawData;
rhi::ImGuiOnDetachFn             ImGuiOnDetach;
rhi::ImguiRenderDrawDataD3D12Fn  ImGuiRenderDrawDataD3D12;



ImguiLayer::ImguiLayer() : Layer("ImguiLayer")
{}


ImguiLayer::~ImguiLayer()
{}

void ImguiLayer::ConnectToEventBusImpl(void* bus)
{
	auto eventBus = static_cast<IFNITY::GLFWEventSource*>(bus);
	if ( eventBus )
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

//TODO : If exist more functions, translate this switch case to extern global function OpenGl to easy maintenance.
void ImguiLayer::OnAttach()
{
	//m_monitor.setDisplay(&m_NvmlDisplayMonitor);

	switch ( App::GetApp().GetGraphicsAPI() )
	{
	case rhi::GraphicsAPI::OPENGL:
		ImGuiRenderDrawData = ImGui_ImplOpenGL3_RenderDrawData;
		ImGuiOnDetach = ImGui_ImplOpenGL3_Shutdown;
		break;
	case rhi::GraphicsAPI::D3D11:
		ImGuiRenderDrawData = ImGui_ImplDX11_RenderDrawData;
		ImGuiOnDetach = ImGui_ImplDX11_Shutdown;
		break;
	case rhi::GraphicsAPI::D3D12:
		ImGuiRenderDrawData = [](ImDrawData* drawData) {
			ImDrawData* draw_data = ImGui::GetDrawData(); }; //This implement in D3D12Render
		ImGuiOnDetach = ImGui_ImplDX12_Shutdown;
		break;
	case rhi::GraphicsAPI::VULKAN:
		ImGuiRenderDrawData = [](ImDrawData* drawData) {   // Obtén los datos de dibujo
			ImDrawData* draw_data = ImGui::GetDrawData();
			}; // This implement in VulkanRender
		ImGuiOnDetach = ImGui_ImplVulkan_Shutdown;
		break;
	default:
		break;
	}

}

void ImguiLayer::OnDetach()
{
	ImGuiOnDetach();
	DestroyImGuiContext();
	
}

void ImguiLayer::OnUpdate()
{

	ImGui::Render();
	ImGuiRenderDrawData(ImGui::GetDrawData());
	
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

void ImguiLayer::onEventReceived(const KeyRelease& event)
{}

void ImguiLayer::onEventReceived(const MouseMove& event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent((float)event.getX(), (float)event.getY());

}

void ImguiLayer::onEventReceived(const ScrollMouseMove& event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent((float)event.getX(), (float)event.getY());



}

void ImguiLayer::onEventReceived(const MouseClick& event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(event.getButton(), event.getState());


}


IFNITY_END_NAMESPACE
