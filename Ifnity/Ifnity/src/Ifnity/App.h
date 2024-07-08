#pragma once
#include <stdio.h>
#include "Core.h"
#include "Window.h"


IFNITY_NAMESPACE
	
//forward declaration
	class WindowCloseEvent;
	class IFNITY_API App
	{
	public:
		App();
		virtual ~App();
		void run();

		void OnEvent(Event& e);

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool m_runing = true;
		std::unique_ptr<Window> m_Window;
	};

	App* CreateApp();

IFNITY_END_NAMESPACE

