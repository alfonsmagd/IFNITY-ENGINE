#include "pch.h"
#include "WindowOpengl.h"
#include <glm\glm.hpp>
#include <Ifnity\Event\WindowEvent.h>

IFNITY_NAMESPACE

///Static variable to check if GLFW is initialized.
static bool s_GLFWInitialized = false;

///Default Constructor
///
/// @param props Window properties.
WindowOpengl::WindowOpengl(const WindowProps& props)
{
	//
	glm::abs(0.0f);
	m_WindowData.props = props;
	m_WindowData.Title = props.Title;
	m_WindowData.VSync = false;

	// Initialize the library
	Init();

}

WindowOpengl::~WindowOpengl()
{
	// Shutdown the window and close GLFW
	Shutdown();
}

void WindowOpengl::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
}

/// <summary>
/// This function sets the VSync in glfw window.
/// </summary>
/// <param name="enabled"> true enable Vsync, false disenabled Vsync.</param>
void WindowOpengl::SetVSync(bool enabled)
{
	//GLFW function to enable or disable VSync
	enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);
}

bool WindowOpengl::IsVSync() const
{
	return false;
}

void WindowOpengl::Init()
{
	// Create the window 
	IFNITY_LOG(LogApp, TRACE, "Initialize Window {0}", m_WindowData.Title.c_str());

	//Check if GLFW is initialized before.
	if(!s_GLFWInitialized)
	{
		// Initialize the library
		if(!glfwInit())
		{
			IFNITY_LOG(LogApp, ERROR, "Cant Initialize GLFW Library ");
		}
		else
		{
			s_GLFWInitialized = true;
		}
	}
	

	// Create a windowed mode window glfwCreateWindow
	m_Window = glfwCreateWindow(
		m_WindowData.props.Width,
		m_WindowData.props.Height,
		m_WindowData.Title.c_str(), 
		NULL,
		NULL);
	
	glfwMakeContextCurrent(m_Window);
	//Take a pointer to the window data information to then return in callbaks. 
	glfwSetWindowUserPointer(m_Window, &m_WindowData);
	SetVSync(true);  

	// Set GLFW callbacks

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.props.Width = width;
			data.props.Height = height;

			// event its the argument of the callback function that we set in the windowOpengl class. 
			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});
}

void WindowOpengl::Shutdown()
{
	// Destroy the window
	glfwDestroyWindow(m_Window);
	// Terminate GLFW
	glfwTerminate();

}





IFNITY_END_NAMESPACE

