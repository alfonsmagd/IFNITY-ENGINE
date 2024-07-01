#include "pch.h"

#include "App.h"
#include "Log.h"
#include "Event/WindowEvent.h"
#include "Window.h"


namespace IFNITY
{

	//Default Constructor;
	App::App()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}
	App::~App()
	{
		printf("~App() \n");
	}

	bool OnWindowResize(IFNITY::WindowResizeEvent& e)
	{
		// Aquí va el código para manejar el evento WindowResizeEvent
		// Por ejemplo, podrías imprimir las nuevas dimensiones de la ventana:
		std::cout << "Window resized to " << e.GetWidth() << "x" << e.GetHeight() << std::endl;
		return true;
	}
	void App::run()
	{

		while(m_runing)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}

	/*	WindowResizeEvent e(100, 20);
		IFNITY::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<IFNITY::WindowResizeEvent>(OnWindowResize);
		IFNITY_LOG(LogApp, ERROR, e);*/
	}

}
