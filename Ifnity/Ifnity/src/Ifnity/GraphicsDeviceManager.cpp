#include "GraphicsDeviceManager.h"
#include "Platform\Windows\DeviceOpengl.h"
#include "Platform\Windows\DeviceD3D11.h"

#ifdef _WINDOWS
#include <ShellScalingApi.h>
#pragma comment(lib, "shcore.lib")
#endif

IFNITY_NAMESPACE


bool GraphicsDeviceManager::CreateWindowSurface(const WindowData& props)
{
#ifdef _WINDOWS
	// this needs to happen before glfwInit in order to override GLFW behavior
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#endif
	m_Props = props;

	if ( !CreateInstance() )
	{
		IFNITY_LOG(LogApp, ERROR, "Failed to create Window Instance");
		return false;
	}

	// GLFW Configuration initialization //GLFW NO API YET SPECIFIED by default GLFW uses OPENGL API. 
	glfwSetErrorCallback([](int error, const char* description)
		{
			IFNITY_LOG(LogApp, ERROR, "GLFW Error ({0}): {1}", error, description);
		});

	glfwDefaultWindowHints(); // optional, the current window hints are already the default

	//GLFW by default format 
	//TODO: Add more formats to the  window creatin with glfwWindowHint and probably more configurations here

	if ( !ConfigureSpecificHintsGLFW() )
	{
		IFNITY_LOG(LogApp, ERROR, "Failed to configure GLFW hints");
		return false;
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);   // Ignored for fullscreen

	m_Window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), nullptr, nullptr);

	//Check if the window was created correctly
	if ( !m_Window )
	{
		IFNITY_LOG(LogApp, ERROR, "Failed to create GLFW window");
		glfwTerminate();
		return false;
	}

	//Configure differente properties of check if window is fullscreen, maximized, minimized, etc. 

		//Take a pointer to the window data information to then return in callbaks. 
	glfwSetWindowUserPointer(m_Window, &m_Props);

	//SET VSYCN .... IN THE FUTRE TODO


	// Set GLFW callbacks
	SetGLFWCallbacks();


	// Set the swapchain and get the surface for API selected. 
	if ( !CreateAPISurface() )
	{
		IFNITY_LOG(LogApp, ERROR, "Failed to create API Surface");
		return false;
	}

	glfwShowWindow(m_Window);

	SetGraphicsDeviceState(StateGraphicsDevice::INITIALIZED);
	return true;


}


bool GraphicsDeviceManager::CreateInstance()
{
	// Initialize the library
	if ( m_InstanceCreated )
	{
		IFNITY_LOG(LogApp, TRACE, "Window Instance already created");
		return true;
	}
	// Check if GLFW can be initialized.
	if ( !glfwInit() )
	{
		IFNITY_LOG(LogApp, ERROR, "Failed to initialize GLFW");
		return false;
	}

	m_InstanceCreated = InitInternalInstance();

}
void GraphicsDeviceManager::Shutdown()
{
	// Chek if Shutdown its provide when API is initialized
	if ( m_StateGraphicsDevice == StateGraphicsDevice::INITIALIZED )
	{

		IFNITY_LOG(LogApp, WARNING, "Shutdown Graphics Device Manager");
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
		glfwTerminate();

		SetGraphicsDeviceState(StateGraphicsDevice::NOT_INITIALIZED);

		m_InstanceCreated = false;
	}
	else
	{
		IFNITY_LOG(LogApp, ERROR, "Graphics Device Manager is not initialized");
	}



}

void GraphicsDeviceManager::RenderDemo(int w, int h) const
{}
// Create Window 
GraphicsDeviceManager* GraphicsDeviceManager::Create(rhi::GraphicsAPI api)
{

	//Check the API type
	switch ( api )
	{
	case rhi::GraphicsAPI::OPENGL:
	{
		return BuildWindow<DeviceOpengl>();

	} // Fin del ámbito para OPENGL
	break;

	case rhi::GraphicsAPI::D3D11:
	{
		return BuildWindow<DeviceD3D11>();
	} // Fin del ámbito para D3D11
	case rhi::GraphicsAPI::D3D12:
	{


	} // Fin del ámbito para D3D12
	break;
	case rhi::GraphicsAPI::VULKAN:
	{

	}
	break;



	default:
		return BuildWindow<DeviceOpengl>();
	}
}

void GraphicsDeviceManager::SetGLFWCallbacks()
{
	// Set GLFW callbacks

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			//data.GLFWEventSourceBus.triggerWindowResize(width, height);
			data.GLFWEventSourceBus.triggerEvent<WindowResize>(width, height);


		});

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			//data.GLFWEventSourceBus.triggerWindowClose();
			data.GLFWEventSourceBus.triggerEvent<WindowClose>();
		});

	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch ( action )
			{
			case GLFW_PRESS:
				//data.GLFWEventSourceBus.triggerKeyPressed(key, 0);
				data.GLFWEventSourceBus.triggerEvent<KeyPressed>(key, 0);
				break;
			case GLFW_RELEASE:
				//data.GLFWEventSourceBus.triggerKeyReleased(key);
				data.GLFWEventSourceBus.triggerEvent<KeyRelease>(key);
				break;
			case GLFW_REPEAT:
				//data.GLFWEventSourceBus.triggerKeyPressed(key, 1);
				data.GLFWEventSourceBus.triggerEvent<KeyPressed>(key, 1);
				break;
			}
		});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.GLFWEventSourceBus.triggerEvent<MouseMove>(xpos, ypos);
		});

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.GLFWEventSourceBus.triggerEvent<ScrollMouseMove>(xoffset, yoffset);
		});

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.GLFWEventSourceBus.triggerEvent<MouseClick>(button, action, mods);
		});


}

IFNITY_END_NAMESPACE