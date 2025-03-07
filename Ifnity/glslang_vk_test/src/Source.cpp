

// IFNITY.cp

#include <Ifnity.h>

using namespace IFNITY;
using namespace IFNITY::rhi;
using vec3 = glm::vec3;



class ExampleLayer: public IFNITY::GLFWEventListener, public IFNITY::Layer
{
public:
	ExampleLayer(): Layer("Example") {}
	~ExampleLayer() {}

	void OnUpdate() override
	{
		IFNITY_LOG(LogApp, INFO, "Update App");
	}


	void onEventReceived(const IFNITY::WindowResize& event) override
	{
		IFNITY_LOG(LogApp, WARNING, event.ToString() + "Example Layer");
	}
	void ConnectToEventBusImpl(void* bus) override
	{
		auto eventBus = static_cast<IFNITY::GLFWEventSource*>(bus);
		if(eventBus)
		{
			CONNECT_EVENT_LAYER(WindowResize, eventBus);
			CONNECT_EVENT_LAYER(WindowClose, eventBus);
			CONNECT_EVENT_LAYER(KeyPressed, eventBus);
			CONNECT_EVENT_LAYER(KeyRelease, eventBus);
			CONNECT_EVENT_LAYER(MouseMove, eventBus);
			CONNECT_EVENT_LAYER(ScrollMouseMove, eventBus);
			CONNECT_EVENT_LAYER(MouseClick, eventBus);
		}
		else
		{
			//Manage conversion pointer error. 
			IFNITY_LOG(LogApp, ERROR, "The pointer is not type IFNITY::GLFWEventSource*");
		}
	}


};

class NVMLLayer: public IFNITY::Layer
{
public:
	NVMLLayer(): Layer("NVML") {}
	~NVMLLayer() {}

	void OnUpdate() override
	{

		monitor.refresh();
		monitor.display();
	}

	void ConnectToEventBusImpl(void* bus) override
	{}
	void OnAttach() override
	{
		loggerDisplayMonitor = LoggerDisplayMonitor();
		monitor.setDisplay(&loggerDisplayMonitor);

		IFNITY_LOG(LogApp, INFO, "NVML Layer is attached");
	}

private:
	NvmlMonitor monitor;
	LoggerDisplayMonitor loggerDisplayMonitor;
};



class ImGuiTestLayer : public IFNITY::Layer
{
public:
	ImGuiTestLayer() : Layer("ImGuiTest") {}
	~ImGuiTestLayer() {}

	void OnAttach() override
	{
		IFNITY_LOG(LogApp, INFO, "ImGuiTest Layer is attached");
	}

	void OnUpdate() override
	{
		ImGuiContext* context = GetImGuiContext();
		if (context == nullptr)
		{
			IFNITY_LOG(LogApp, ERROR, "Failed to get ImGui context from DLL");
			return;
		}
		ImGui::SetCurrentContext(context);

		ChooseApi();
		//IFNITY_LOG(LogApp, INFO, "Update ImGuiTest Layer OnUpdate");
	}
	// Heredado vía Layer
	void ConnectToEventBusImpl(void* bus) override
	{
		

	}
private:
	// Una función que se llama al hacer clic en el botón
	void AccionPorOpcion(int opcionSeleccionada) {

		GraphicsAPI api = IFNITY::App::GetApp().GetGraphicsAPI();
		switch (opcionSeleccionada) {
		case 0:
			// Acción para la opción 1
			IFNITY_LOG(LogApp, INFO, "OPENGL");
			

			IFNITY::App::GetApp()
				.SetGraphicsAPI(GraphicsAPI::OPENGL,api != GraphicsAPI::OPENGL);
			break;
		case 1:
			// Acción para la opción 2
			IFNITY_LOG(LogApp, INFO, "D3D11");
			IFNITY::App::GetApp()
				.SetGraphicsAPI(GraphicsAPI::D3D11, api != GraphicsAPI::D3D11);
			break;

		case 2:
			// Acción para la opción 3
			IFNITY_LOG(LogApp, INFO, "D3D12");
			IFNITY::App::GetApp()
				.SetGraphicsAPI(GraphicsAPI::D3D12, api != GraphicsAPI::D3D12);
			break;

		case 3:
			// Acción para la opción 3
			IFNITY_LOG(LogApp, INFO, "VULKAN");
			IFNITY::App::GetApp()
				.SetGraphicsAPI(GraphicsAPI::VULKAN, api != GraphicsAPI::VULKAN);
			break;
		default:
		
			break;
		}
	}

	void ChooseApi() {
		static int selectOption = 0;
		const char* options[] = { "OPENGL", "D3D11","D3D12","VULKAN"};

		ImGui::Begin("API WINDOW");  // Comienza la creación de la ventana

		// Combo box con las opciones
		if (ImGui::Combo("Choose Option ", &selectOption, options, IM_ARRAYSIZE(options))) {
			// Este bloque se ejecuta cada vez que se selecciona una opción diferente
		}

		// Botón que ejecuta la función cuando se hace clic
		if (ImGui::Button("OK")) {
			AccionPorOpcion(selectOption);
		}

		ImGui::End();  // Termina la creación de la ventana
	}






	
};

class Source: public IFNITY::App
{
private:
	BufferHandle m_UBO;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_SolidPipeline;
	GraphicsPipelineHandle m_WireFramePipeline;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;
public:
	Source(IFNITY::rhi::GraphicsAPI api) : IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr())
	{
		// Obtener el contexto de ImGui desde IFNITY  DLL
		/*ImGuiContext* context = GetImGuiContext();
		if (context == nullptr)
		{
			IFNITY_LOG(LogApp, ERROR, "Failed to get ImGui context from DLL");
			return;
		}*/

		// Establecer el contexto de ImGui en la aplicación principal
		//ImGui::SetCurrentContext(context);
		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 
		
		

	}

	void Initialize() override
	{
		//File System Instance get it 
		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);
		vfs.Mount("test", "Shaders/testShader", IFNITY::FolderType::NO_DEFINED);

		auto files = vfs.ListFilesInCurrentDirectory("test");
		auto* rdevice = m_ManagerDevice->GetRenderDevice();
		auto getFileName = [ &files ](const std::string& extension) -> const char*
			{
				for(const auto& file : files)
				{
					if(file.size() >= extension.size() &&
						file.compare(file.size() - extension.size(), extension.size(), extension) == 0)
					{
						return file.c_str();
					}
				}
				return nullptr;
			};

		IFNITY::testShaderCompilation(getFileName(".vert"), "Shaders/testShader/main.vert.spv");
		IFNITY::testShaderCompilation(getFileName(".frag"), "Shaders/testShader/main.frag.spv");

		IFNITY_LOG(LogApp, INFO, "END APP ONLY TEST SHADER BUILDING ");


		
		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.NoCompile = true;
			//DescriptionShader.FileName = "triangle01.vert";
			DescriptionShader.FileName = "glm.vert";
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			//DescriptionShader.FileName = "triangle01.frag";
			DescriptionShader.FileName = "glm.frag";
			m_ps->SetShaderDescription(DescriptionShader);
		}

		ShaderCompiler::CompileShader(m_vs.get());
		ShaderCompiler::CompileShader(m_ps.get());

		const uint32_t wireframe = 1;
		GraphicsPipelineDescription gdesc1;
		GraphicsPipelineDescription gdesc2;

		BufferDescription desc;
		desc.size = sizeof(glm::mat4);
		desc.type = BufferType::CONSTANT_BUFFER;
		desc.binding = 0;
		desc.data = nullptr;

		m_UBO = rdevice->CreateBuffer(desc);

		{
			gdesc1.SetVertexShader(m_vs.get()).
				  SetPixelShader(m_ps.get()).
				  AddDebugName("Wireframe Pipeline").
				  SetRasterizationState({ .cullMode = rhi::CullModeType::Front ,.polygonMode = rhi::PolygonModeType::Line }).
				  AddSpecializationConstant({ .id = 0, .size = sizeof(uint32_t) , .dataSize = sizeof(wireframe),.data = &wireframe , });

			//GraphicsPipelineDescription gdesc
			m_WireFramePipeline = rdevice->CreateGraphicsPipeline(gdesc1);
		}

		{
			gdesc2.SetVertexShader(m_vs.get()).
				  SetPixelShader(m_ps.get()).
				  AddDebugName("Solid Pipeline").
				  SetRasterizationState({ .cullMode = rhi::CullModeType::Front ,.polygonMode = rhi::PolygonModeType::Fill });

			m_SolidPipeline = rdevice->CreateGraphicsPipeline(gdesc2);
		}

		m_SolidPipeline->BindPipeline(rdevice);
		
	
		//exit(0);

	}
	
	void Render() override
	{
		auto* rdevice = m_ManagerDevice->GetRenderDevice();


		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());

		const mat4 m = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -3.5f)), (float)glfwGetTime(), vec3(1.0f, 1.0f, 1.0f));
		const mat4 p = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);


		//StartRecording
		rdevice->StartRecording();
		
		rdevice->WriteBuffer(m_UBO, glm::value_ptr(p*m), sizeof(glm::mat4));

		DrawDescription desc;
		rdevice->DrawObject(m_SolidPipeline, desc);
		rdevice->DrawObject(m_WireFramePipeline, desc);
		
		rdevice->StopRecording();
		//StopRecording 


		IFNITY_LOG(LogApp, INFO, "Render App");
	}
	
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}
	~Source() override {}
};

class Source_TestD3D12 : public IFNITY::App
{
public:
	Source_TestD3D12(IFNITY::rhi::GraphicsAPI api) : IFNITY::App(api)
	{
		PushLayer(new ExampleLayer());
	}
	~Source_TestD3D12() override
	{
	}
};


IFNITY::App* IFNITY::CreateApp()
{


	auto api = IFNITY::rhi::GraphicsAPI::VULKAN;


	//return new Source_TestD3D12(api);
	return new Source(api);
}

