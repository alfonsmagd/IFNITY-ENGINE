#include "Window.h"
#include "Platform\Windows\WindowOpengl.h"

IFNITY_NAMESPACE




Window* Window::Create(API_WINDOW_TYPE api, const WindowProps& props)
{

	//Check the API type
	switch(api)
	{
		case OPENGL:
		{
			return BuildWindow<WindowOpengl>(props);
			
		} // Fin del �mbito para OPENGL

	case D3D12:
	{


	} // Fin del �mbito para D3D12
	break; 

	// Otros cases...

	default:
		 return BuildWindow<WindowOpengl>(props);;
	}
}

IFNITY_END_NAMESPACE