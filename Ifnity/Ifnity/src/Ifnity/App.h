#pragma once
#include <stdio.h>
#include "Core.h"
#include "Window.h"

IFNITY_NAMESPACE
	
	class IFNITY_API App
	{
	public:
		App();
		virtual ~App();
		void run();

	private:
		bool m_runing = true;
		std::unique_ptr<Window> m_Window;
	};

	App* CreateApp();

IFNITY_END_NAMESPACE

