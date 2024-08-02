#include "Window.h"
#include "Platform\Windows\WindowOpengl.h"


#ifdef _WINDOWS
#include <ShellScalingApi.h>
#pragma comment(lib, "shcore.lib")
#endif

IFNITY_NAMESPACE


void Window::CreateWindowSurface(const WindowProps& props)
{
#ifdef _WINDOWS
	// this needs to happen before glfwInit in order to override GLFW behavior
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#endif
	m_Props = props;

	if(!CreateInstance())
	{
		IFNITY_LOG(LogApp, ERROR, "Failed to create Window Instance");
		return;
	}
}


bool Window::CreateInstance()
{
	// Initialize the library
	if(m_InstanceCreated)
	{
		IFNITY_LOG(LogApp, TRACE, "Window Instance already created");
		return true;
	}
	// Check if GLFW can be initialized.
	if(!glfwInit())
	{
		IFNITY_LOG(LogApp, ERROR, "Failed to initialize GLFW");
		return false ;
	}

	m_InstanceCreated = InitInternalInstance();

}
// Create Window 
Window* Window::Create(rhi::GraphicsAPI api, const WindowProps& props)
{

	//Check the API type
	switch(api)
	{
	case rhi::GraphicsAPI::OPENGL:
		{
			return BuildWindow<WindowOpengl>(props);
			
		} // Fin del ámbito para OPENGL
		break;
	case rhi::GraphicsAPI::D3D12:
		{


		} // Fin del ámbito para D3D12
		break;
	case rhi::GraphicsAPI::VULKAN:
		{
		
		}
		break; 

		

		default:
			 return BuildWindow<WindowOpengl>(props);
		}
}

IFNITY_END_NAMESPACE