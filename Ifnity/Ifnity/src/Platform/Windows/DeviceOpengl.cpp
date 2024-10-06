#include "pch.h"
#include "DeviceOpengl.h"
#include "Platform/ImguiRender/ImguiOpenglRender.h"
#include <glm\glm.hpp>



IFNITY_NAMESPACE

///Static variable to check if GLFW is initialized.
static bool s_GLFWInitialized = false;



DeviceOpengl::~DeviceOpengl()
{
	// Shutdown the window and close GLFW
	//Other function to close the window
	IFNITY_LOG(LogApp, WARNING, "DeviceOpenGL destroyed");
}

void DeviceOpengl::OnUpdate()
{
	//Clear the color buffer
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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

bool DeviceOpengl::InitializeDeviceAndContext()
{
	if(m_Window == nullptr)
	{
		IFNITY_LOG(LogApp, ERROR, "Failed to get Window in OPENGL InitializeDeviceAndContext()");
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

void DeviceOpengl::ResizeSwapChain()
{}

void DeviceOpengl::InitializeGui()
{
	ImGui_ImplOpenGL3_Init("#version 450"); // TODO: Change version 450 to a variable that can be
	// changed in the future.
	IFNITY_LOG(LogCore, TRACE, "Imgui API is set to OpenGL");
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

// sp is Shader Fragment. 
void DeviceOpengl::DemoTriangle(const char* sv, const char* sp)
{
	const GLuint shaderVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shaderVertex, 1, &sv, nullptr);
	glCompileShader(shaderVertex);

	const GLuint shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shaderFragment, 1, &sp, nullptr);
	glCompileShader(shaderFragment);

	const GLuint program = glCreateProgram();
	glAttachShader(program, shaderVertex);
	glAttachShader(program, shaderFragment);

	glLinkProgram(program);
	glUseProgram(program);

	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void DeviceOpengl::RenderDemo(int w,int h) const
{

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Frame Start");
	glViewport(0, 0,w , h);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glPopDebugGroup();
}


std::string DeviceOpengl::GetOpenGLInfo() const
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

void DeviceOpengl::Shutdown()
{
	IFNITY_LOG(LogApp, WARNING, "Shutdown Init OPENGL");
	GraphicsDeviceManager::Shutdown();

}

void DeviceOpengl::InternalPreDestroy()
{
}



IFNITY_END_NAMESPACE

