#include "pch.h"
#include "WindowWindow.h"


IFNITY_NAMESPACE

///Static variable to check if GLFW is initialized.
static bool s_GLFWInitialized = false;

Window* Window::Create(const WindowProps& props)
{
	return new WindowWindow(props);
}


///Default Constructor
///
/// @param props Window properties.
WindowWindow::WindowWindow(const WindowProps& props)
{
	//
	m_WindowData.props = props;
	m_WindowData.Title = props.Title;
	m_WindowData.VSync = false;

	// Initialize the library
	Init();

}

WindowWindow::~WindowWindow()
{
	// Shutdown the window and close GLFW
	Shutdown();
}

void WindowWindow::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
}

/// <summary>
/// This function sets the VSync in glfw window.
/// </summary>
/// <param name="enabled"> true enable Vsync, false disenabled Vsync.</param>
void WindowWindow::SetVSync(bool enabled)
{
	//GLFW function to enable or disable VSync
	enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);
}

bool WindowWindow::IsVSync() const
{
	return false;
}

void WindowWindow::Init()
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
	glfwSetWindowUserPointer(m_Window, &m_WindowData.props);
	SetVSync(true);
}

void WindowWindow::Shutdown()
{
	// Destroy the window
	glfwDestroyWindow(m_Window);
	// Terminate GLFW
	glfwTerminate();

}





IFNITY_END_NAMESPACE

