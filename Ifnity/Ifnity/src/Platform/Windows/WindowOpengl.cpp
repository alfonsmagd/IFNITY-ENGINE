#include "pch.h"
#include "WindowOpengl.h"
#include <glm\glm.hpp>


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

bool WindowOpengl::ConfigureSpecificHintsGLFW() const
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

	return true;
}

bool WindowOpengl::CreateAPISurface()
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

			switch(action)
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

	//Initialize GLAD
	InitializeGLAD();

	//Print OpenGL information
	IFNITY_LOG(LogApp, WARNING, GetOpenGLInfo().c_str());
}

void WindowOpengl::InitializeGLAD()
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

std::string WindowOpengl::GetOpenGLInfo()
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

void WindowOpengl::Shutdown()
{
	// Destroy the window
	glfwDestroyWindow(m_Window);
	// Terminate GLFW
	glfwTerminate();

}


IFNITY_END_NAMESPACE

