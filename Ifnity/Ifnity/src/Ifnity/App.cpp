#include "pch.h"

#include "App.h"
#include "GraphicsDeviceManager.h"
#include <glad\glad.h>
#include <GLFW/glfw3.h>
#include "Platform/ImguiRender/ImguiOpenglRender.h"
#include "Ifnity/Layers/ExampleLayer.h" //TODO BORRAR , LO SUYO SU CPP CORRESPONDIENTE
#include "Ifnity/Layers/NVML_Layer.hpp"




namespace IFNITY
{


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


		// Initialize ImGui
		
		InitializeImGui();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiBackendFlags_HasMouseCursors; // Enable SetMousePos.
		io.ConfigFlags |= ImGuiBackendFlags_HasSetMousePos; // Enable SetMousePos.
		io.FontGlobalScale = 1.0f;
		ImGui::StyleInfity();					// Clasic color style. 

		//Classic version  1.87 see IMGUI_DISABLE_OBSOLETE_KEYIO in new version 
		// not necessary intialization maps for keys. 
		//io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;					

		ImGui_ImplOpenGL3_Init("#version 450");


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
		
			//Imgui Render Frame 
			{}
			ImGuiIO& io = ImGui::GetIO();
			App& app = App::GetApp();

			io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
			IFNITY_LOG(LogApp, INFO, "Width: " + std::to_string(app.GetWindow().GetWidth()) + " Height: " + std::to_string(app.GetWindow().GetHeight()));

			float time = (float)glfwGetTime();

			io.DeltaTime = m_Time > 0.0 ? (float)(time - m_Time) : (float)(1.0f / 60.0f);
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();
			ImPlot::CreateContext();
			ImPlot::ShowDemoWindow();
			ImGui::Text("Hello, world %d", 123);
			if (ImGui::Button("Save"))
				static bool show = true;
			//Mostrar numeros aleatorios en imgui en texto 

					//Crear aleatorio 
			int random = rand() % 100 + 1;
			ImGui::Text("Random number: %d", random);
		



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
