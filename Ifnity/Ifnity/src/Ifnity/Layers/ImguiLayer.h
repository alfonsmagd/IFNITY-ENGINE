


#pragma once
#include "pch.h"
#include "Ifnity/Layer.hpp"
#include "Ifnity/Event/WindowEvent.h"
#include <BenchMark\NvmlMonitor.hpp>
#include "ImGuiContextManager.h"



IFNITY_NAMESPACE

class IFNITY_API ImguiLayer : public Layer , public GLFWEventListener
{

public:
		ImguiLayer();
		~ImguiLayer();

		void ConnectToEventBusImpl(void* bus) override;
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;

		void onEventReceived(const WindowResize& event) override;
		void onEventReceived(const KeyPressed& event) override;
		void onEventReceived(const KeyRelease& event)override;
		void onEventReceived(const MouseMove& event) override;
		void onEventReceived(const ScrollMouseMove& event)override;
		void onEventReceived(const MouseClick& event)override;
		
private:
	 float m_Time = 0.0f;
	 //NvmlMonitor m_monitor{ 100,5 };
	 ImguiNVML m_NvmlDisplayMonitor;
};


IFNITY_END_NAMESPACE