#include "pch.h"

#include "App.h"
#include "GraphicsDeviceManager.h"
#include <glad\glad.h>
#include <GLFW/glfw3.h>




namespace IFNITY
{

	std::vector<char> ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if(!file.is_open())
		{
			std::cerr << "Error al abrir el archivo: " << filename << std::endl;
			return {};
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}


	// Función para cargar un shader SPIR-V
	// Función para cargar un shader SPIR-V
	GLuint LoadSPIRVShader(GLenum shaderType, const std::string& filename, const char* entryPoint)
	{
		// Leer el archivo SPIR-V
		std::vector<char> spirvCode = ReadFile(filename);
		if(spirvCode.empty())
		{
			std::cerr << "Error al leer el archivo SPIR-V: " << filename << std::endl;
			return 0;
		}

		// Crear el shader
		GLuint shader = glCreateShader(shaderType);
		if(shader == 0)
		{
			std::cerr << "Error al crear el shader" << std::endl;
			return 0;
		}

		// Cargar el código SPIR-V en el shader
		glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, spirvCode.data(), spirvCode.size());
		glSpecializeShader(shader, entryPoint, 0, nullptr, nullptr);

		// Comprobar errores de compilación
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if(!compiled)
		{
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::vector<char> infoLog(infoLogLength);
			glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());
			std::cerr << "Error al compilar el shader: " << infoLog.data() << std::endl;
			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}


	// Función para crear un programa de shader
	GLuint CreateShaderProgram(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
	{
		// Cargar los shaders SPIR-V
		GLuint vertexShader = LoadSPIRVShader(GL_VERTEX_SHADER, vertexShaderFile, "main_vs");
		GLuint fragmentShader = LoadSPIRVShader(GL_FRAGMENT_SHADER, fragmentShaderFile, "main_ps");
		if(vertexShader == 0 || fragmentShader == 0)
		{
			return 0;
		}

		// Crear el programa de shader
		GLuint program = glCreateProgram();
		if(program == 0)
		{
			std::cerr << "Error al crear el programa de shader" << std::endl;
			return 0;
		}

		// Vincular los shaders al programa
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		// Comprobar errores de enlace
		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if(!linked)
		{
			GLint infoLogLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::vector<char> infoLog(infoLogLength);
			glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());
			std::cerr << "Error al enlazar el programa de shader: " << infoLog.data() << std::endl;
			glDeleteProgram(program);
			return 0;
		}

		// Los shaders ya no son necesarios después de enlazarlos al programa
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return program;
	}










	static const char* shaderCodeVertex = R"(

#version 450

const vec2 _31[3] = vec2[](vec2(-0.5), vec2(0.0, 0.5), vec2(0.5, -0.5));
const vec3 _35[3] = vec3[](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));

layout(location = 0) out vec3 out_var_COLOR;

void main()
{
    gl_Position = vec4(_31[uint(gl_VertexID)], 0.0, 1.0);
    out_var_COLOR = _35[uint(gl_VertexID)];
}




)";

	static const char* shaderCodeFragment = R"(
#version 450


layout(location = 0) in vec3 in_var_COLOR;
layout(location = 0) out vec4 out_var_SV_Target0;

void main()
{
    out_var_SV_Target0 = vec4(in_var_COLOR, 1.0);
}




)";


	//Static member  declaration
	App* App::s_Instance = nullptr;
	//Default Constructor;
	App::App()
	{
		s_Instance = this;
		// Create windows props 
		WindowData props;

		m_Window = std::unique_ptr<GraphicsDeviceManager>(GraphicsDeviceManager::Create(rhi::GraphicsAPI::OPENGL));

		m_Window->CreateWindowSurface(props);
		//Intialize the EventListenerControler 
		m_GLFWEventListener = std::make_unique<GLFWEventListener>();



		SetEventBus(m_Window->GetGLFWEventSource());

		CONNECT_EVENT(WindowResize);
		CONNECT_EVENT(WindowClose);
		CONNECT_EVENT(KeyPressed);
		CONNECT_EVENT(KeyRelease);
		CONNECT_EVENT(MouseMove);
		CONNECT_EVENT(ScrollMouseMove);
		CONNECT_EVENT(MouseClick);





	}
	App::~App()
	{

		//OnDetach all layers
		for(Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
		}
		s_Instance = nullptr;
		IFNITY_LOG(LogApp, INFO, "App is destroyed");

	}


	void App::run()
	{
		InitiateEventBusLayers();

		const GLuint shaderVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shaderVertex, 1, &shaderCodeVertex, nullptr);
		glCompileShader(shaderVertex);

		const GLuint shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shaderFragment, 1, &shaderCodeFragment, nullptr);
		glCompileShader(shaderFragment);

		const GLuint program = glCreateProgram();
		glAttachShader(program, shaderVertex);
		glAttachShader(program, shaderFragment);

		glLinkProgram(program);
		glUseProgram(program);

		GLuint vao;
		glCreateVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//// Cargar y crear el programa de shader
		//GLuint shaderProgram = CreateShaderProgram("D:\\IFNITY-ENGINE\\Ifnity\\Ifnity\\shaders_main_vs.bin", "D:\\IFNITY-ENGINE\\Ifnity\\Ifnity\\shaders_main_ps.bin");
		//if(shaderProgram == 0)
		//{
		//	std::cerr << "Error al crear el programa de shader" << std::endl;
		//	
		//}
	

		//// Usar el programa de shader
		//glUseProgram(shaderProgram);

		
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		while(isRunning())
		{
			// Iniciar una sección de depuración
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Frame Start");
			glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
			glClear(GL_COLOR_BUFFER_BIT);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glPopDebugGroup();
		
			for(Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
			}


			m_Window->OnUpdate();
		}


		m_Window->Shutdown();
	}

	void App::PushLayer(Layer* layer)
	{

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();


	}

	void App::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();

	}

	void App::InitiateEventBusLayers()
	{
		for(Layer* layer : m_LayerStack)
		{
			layer->ConnectToEventBus(m_EventBus);
		}

	}

	bool App::isRunning() const
	{
		return m_GLFWEventListener->getRunning();
	}


}
