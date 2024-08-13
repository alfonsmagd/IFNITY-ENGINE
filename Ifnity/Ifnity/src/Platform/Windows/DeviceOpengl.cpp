#include "pch.h"
#include "DeviceOpengl.h"
#include <glm\glm.hpp>



IFNITY_NAMESPACE

///Static variable to check if GLFW is initialized.
static bool s_GLFWInitialized = false;



DeviceOpengl::~DeviceOpengl()
{
	// Shutdown the window and close GLFW
	//Other function to close the window

}

void DeviceOpengl::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
}

/// <summary>
/// This function sets the VSync in glfw window.
/// </summary>
/// <param name="enabled"> true enable Vsync, false disenabled Vsync.</param>
void DeviceOpengl::SetVSync(bool enabled)
{
	//GLFW function to enable or disable VSync
	enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);
}

bool DeviceOpengl::IsVSync() const
{
	return false;
}

bool DeviceOpengl::ConfigureSpecificHintsGLFW() const
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

	return true;
}

bool DeviceOpengl::CreateAPISurface()
{
	if(m_Window == nullptr)
	{
		IFNITY_LOG(LogApp, ERROR, "Failed to get Window in OPENGL CreateAPISurface()");
		glfwTerminate();
		return false;
	}

	// TODO TO GET THE SURFACE FOR OPENGL API , and not setting by default. 
	glfwMakeContextCurrent(m_Window);

	//Initialize GLAD
	InitializeGLAD();

	SetVSync(true);
	//Print OpenGL information
	IFNITY_LOG(LogApp, WARNING, GetOpenGLInfo().c_str());

	return true;

	

}

//NOT USE ..
void DeviceOpengl::Init()
{
	
}

void DeviceOpengl::InitializeGLAD()
{
	// Initialize glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		IFNITY_LOG(LogApp, ERROR, "Failed to initialize GLAD");
		
	}
	else
	{
		IFNITY_LOG(LogApp, WARNING, "GLAD Initialized");
		const GLubyte* vendor = glGetString(GL_VENDOR);
		m_IsNvidia = (std::string(reinterpret_cast<const char*>(vendor)).find("NVIDIA") != std::string::npos);
	}
}

std::string DeviceOpengl::GetOpenGLInfo()
{

		std::ostringstream oss;

		//Get version Opengl 
		const GLubyte* renderer = glGetString(GL_RENDERER); 
		const GLubyte* version  =  glGetString(GL_VERSION);   //Opengl Version

		oss << "Renderer: " << renderer << "\n";
		oss << "OpenGL version supported: " << version << "\n";

		// Obtener el número de extensiones soportadas
		GLint numExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

		oss << "Number of extensions: " << numExtensions << "\n";

		// Listar todas las extensiones soportadas
		for(GLint i = 0; i < numExtensions; ++i)
		{
			const GLubyte* extension = glGetStringi(GL_EXTENSIONS, i);
			oss << "Extension " << i + 1 << ": " << extension << "\n";
		}

		return oss.str();
	
}




IFNITY_END_NAMESPACE

