

// IFNITY.cp


#include <Ifnity.h>
#include <future>



using namespace IFNITY;
using namespace IFNITY::rhi;
using glm::vec3;
using glm::vec4;
using glm::vec2;
using glm::mat4;

#define SELECT_API_SELECTOR 0

using namespace IFNITY::rhi;

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



class ImGuiTestLayer: public IFNITY::Layer
{
public:
	ImGuiTestLayer(): Layer("ImGuiTest") {}
	~ImGuiTestLayer() {}

	void OnAttach() override
	{
		IFNITY_LOG(LogApp, INFO, "ImGuiTest Layer is attached");
	}

	void OnUpdate() override
	{
		ImGuiContext* context = GetImGuiContext();
		if(context == nullptr)
		{
			IFNITY_LOG(LogApp, ERROR, "Failed to get ImGui context from DLL");
			return;
		}
		ImGui::SetCurrentContext(context);

		#if SELECT_API_SELECTOR
		ChooseApi();
		#endif
		//IFNITY_LOG(LogApp, INFO, "Update ImGuiTest Layer OnUpdate");
	}
	// Heredado vía Layer
	void ConnectToEventBusImpl(void* bus) override
	{


	}
private:
	// Una función que se llama al hacer clic en el botón
	void AccionPorOpcion(int opcionSeleccionada)
	{

		GraphicsAPI api = IFNITY::App::GetApp().GetGraphicsAPI();
		switch(opcionSeleccionada)
		{
		case 0:
			// Acción para la opción 1
			IFNITY_LOG(LogApp, INFO, "OPENGL");


			IFNITY::App::GetApp()
				.SetGraphicsAPI(GraphicsAPI::OPENGL, api != GraphicsAPI::OPENGL);
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

	void ChooseApi()
	{
		static int selectOption = 0;
		const char* options[] = { "OPENGL", "D3D11","D3D12","VULKAN" };

		ImGui::Begin("API WINDOW");  // Comienza la creación de la ventana

		// Combo box con las opciones
		if(ImGui::Combo("Choose Option ", &selectOption, options, IM_ARRAYSIZE(options)))
		{
			// Este bloque se ejecuta cada vez que se selecciona una opción diferente
		}

		// Botón que ejecuta la función cuando se hace clic
		if(ImGui::Button("OK"))
		{
			AccionPorOpcion(selectOption);
		}

		ImGui::End();  // Termina la creación de la ventana
	}







};

class Source: public IFNITY::App
{
public:
	Source(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api)
	{

		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushOverlay(new IFNITY::ImguiLayer());


	}

	void Initialize() override
	{
		std::string  filenameAssimp = "data/rubber_duck/scene.gltf";
		std::string  meshDataTest = "data/rubber_duck/scene.gltf.meshdata";
		std::string  test = "data/rubber_duck/test.meshes";
		std::string test3 = "data/bistro/Exterior/exterior.obj";

		m_ManagerDevice = &App::GetApp().GetManagerDevice();


		IFNITY::ShaderCompiler::Initialize();


		std::wstring shaderSource3 = LR"(

		cbuffer PerFrameData : register(b0)
		{
			matrix MVP;
		};
		
		struct VertexInput
		{
		    float3 position : POSITION0;
		    float3 color : COLOR1; // Color de entrada desde el vértice
		};
		
		// Función del Vertex Shader
		void main_vs(
		    VertexInput input,                 // Entrada del vértice
		    uint i_vertexId : SV_VertexID,     // ID del vértice
		    out float4 o_pos : SV_Position,     // Salida de posición
		    out float3 o_color : COLOR          // Salida de color
		)
		{
		    // Multiplicar la posición por la matriz MVP
		    o_pos = mul(float4(input.position, 1.0), MVP);
		    // Asignar el color desde la entrada del vértice
		    o_color = input.color; // Usa el color directamente de la entrada
		}
		
		// Función del Pixel Shader
		void main_ps(
		    float4 i_pos : SV_Position,          // Entrada de posición (no se usa en este caso)
		    float3 i_color : COLOR,              // Entrada de color
		    out float4 o_color : SV_Target0       // Salida de color
		)
		{
		    // Asignar el color a la salida del pixel
		    o_color = float4(i_color, 1.0); 
		}
)";



		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.EntryPoint = L"main_vs";
			DescriptionShader.Profile = L"vs_6_0";
			DescriptionShader.Type = ShaderType::VERTEX_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = shaderSource3;
			DescriptionShader.FileName = "vsTriangle";
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.EntryPoint = L"main_ps";
			DescriptionShader.Profile = L"ps_6_0";
			DescriptionShader.Type = ShaderType::PIXEL_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = shaderSource3;
			DescriptionShader.FileName = "psTriangle";
			m_ps->SetShaderDescription(DescriptionShader);
		}

		ShaderCompiler::CompileShader(m_vs.get());
		ShaderCompiler::CompileShader(m_ps.get());


		GraphicsPipelineDescription gdesc;
		gdesc.SetVertexShader(m_vs.get()).
			SetPixelShader(m_ps.get());

		m_GraphicsPipeline = m_ManagerDevice->GetRenderDevice()->CreateGraphicsPipeline(gdesc);

		BufferDescription DescriptionBuffer;
		DescriptionBuffer.SetBufferType(BufferType::CONSTANT_BUFFER)
			.SetByteSize(sizeof(mat4))
			.SetDebugName("UBO MVP")
			.SetStrideSize(0);

		m_UBO = m_ManagerDevice->GetRenderDevice()->CreateBuffer(DescriptionBuffer);

		/*MeshObjectDescription meshAssimp = 
		{
			.filePath = test3,
			.isLargeMesh = true,
			.isGeometryModel = false,
			.meshData = MeshData{},
			.meshFileHeader = MeshFileHeader{},
			.meshDataBuilder = nullptr
		};

		meshAssimp.setMeshDataBuilder(new MeshDataBuilderAssimp(8, 0.01f));*/


		/*loadMeshData(test.c_str(), meshAssimp.meshData);*/

		
		// Iniciar la carga de la malla en un hilo separado
		meshLoadingFuture = std::async(std::launch::async, &Source::loadMeshAsync, this, test3);



	}



	void Render() override
	{
		// Verificar si la carga de la malla ha finalizado
		if(meshLoadingFuture.valid() && meshLoadingFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			// La malla ha sido cargada, puedes renderizarla
			// Aquí iría el código para renderizar la malla
			IFNITY_LOG(LogApp, INFO, "Mesh loaded, ready to render");
		}
		else
		{
			// La malla aún se está cargando
			IFNITY_LOG(LogApp, INFO, "Mesh is still loading...");
		}
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}
private:

	void loadMeshAsync(const std::string& filePath)
	{
		MeshObjectDescription meshAssimp =
		{
			.filePath = filePath,
			.isLargeMesh = true,
			.isGeometryModel = false,
			.meshData = MeshData{},
			.meshFileHeader = MeshFileHeader{},
			.meshDataBuilder = nullptr
		};

		meshAssimp.setMeshDataBuilder(new MeshDataBuilderAssimp(8, 0.01f));
		loadMeshData(filePath.c_str(), meshAssimp.meshData);
		m_meshData = meshAssimp.meshData;










	}
	MeshData m_meshData;
	BufferHandle m_UBO;
	BufferHandle m_VertexBuffer;
	std::future<void> meshLoadingFuture;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_GraphicsPipeline;
	MeshObjectHandle m_MeshTetrahedron;
	MeshObjectHandle m_MeshCube;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;


};

class Source_TestD3D12: public IFNITY::App
{
public:
	Source_TestD3D12(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api)
	{
		PushLayer(new ExampleLayer());
	}
	~Source_TestD3D12() override
	{}
};


IFNITY::App* IFNITY::CreateApp()
{


	auto api = IFNITY::rhi::GraphicsAPI::OPENGL;


	//return new Source_TestD3D12(api);
	return new Source(api);
}

