

// IFNITY.cp

#include <Ifnity.h>



using namespace IFNITY;
using namespace IFNITY::rhi;
using glm::vec3;
using glm::vec4;
using glm::vec2;
using glm::mat4;

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

		//ChooseApi();
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

		MeshObjectDescription cubeDesc;
		cubeDesc.setIsGeometryModel(true).
			setIsLargeMesh(false);
			

		MeshObjectDescription tetrahedronDesc;
		tetrahedronDesc.setIsGeometryModel(true).
			setIsLargeMesh(false);
		




		m_MeshTetrahedron = m_ManagerDevice->GetRenderDevice()->CreateMeshObject(tetrahedronDesc, new MeshDataBuilderGeometryModel(GeometricalModelType::CUBE));
		m_MeshCube = m_ManagerDevice->GetRenderDevice()->CreateMeshObject(cubeDesc,               new MeshDataBuilderGeometryModel(GeometricalModelType::TETHAHEDRON)	);



	}

	void Render() override
	{
		using namespace math;
		// SetPipelineState
		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());
		const mat4 fg = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);

		for(int i = 0; i < 16; ++i)
		{
	

			// Para el cubo
			float angleCube = glm::radians(60.0f * i);
			vec3 positionCube = vec3(cos(angleCube) * 2.0f, sin(angleCube) * 2.0f, -3.5f);
			mat4 modelCube = glm::scale(glm::translate(mat4(1.0f), positionCube), vec3(0.2f));
			modelCube = glm::rotate(modelCube, (float)glfwGetTime(), vec3(1.0f, 1.0f, 1.0f));
			mat4 mvpCube = fg * modelCube;

			m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, glm::value_ptr(mvpCube), sizeof(mvpCube));
			m_MeshCube.get()->Draw();

			// Para el tetraedro
			float angleTetrahedron = glm::radians(25.0f * i);
			vec3 positionTetrahedron = vec3(cos(angleTetrahedron) * 2.0f, sin(angleTetrahedron) * 2.0f, -3.5f);
			mat4 modelTetrahedron = glm::scale(glm::translate(mat4(1.0f), positionTetrahedron), vec3(0.2f));
			modelTetrahedron = glm::rotate(modelTetrahedron, (float)glfwGetTime(), vec3(1.0f, 1.0f, 1.0f));
			mat4 mvpTetrahedron = fg * modelTetrahedron;

			m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, glm::value_ptr(mvpTetrahedron), sizeof(mvpTetrahedron));
			m_MeshTetrahedron.get()->Draw();
			
		}

		IFNITY_LOG(LogApp, INFO, "Render App");
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}
	~Source() override {}


private:
	BufferHandle m_UBO;
	BufferHandle m_VertexBuffer;
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

