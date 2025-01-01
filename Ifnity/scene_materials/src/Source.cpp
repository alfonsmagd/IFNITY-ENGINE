

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
	#define GRID  0 
	#define SCENE 1
	#define kBufferIndex_PerFrame 0

	struct PerFrameData
	{
		mat4 view;
		mat4 proj;
		vec4 cameraPos;
	};

private:
	SceneObjectHandler m_SceneObject;
	MeshObjectHandle m_MeshObject;
	BufferHandle m_UBO;
	BufferHandle m_StorageBuffer;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_GraphicsPipeline[ 2 ];
	std::shared_ptr<IShader> m_vs[ 2 ];
	std::shared_ptr<IShader> m_ps[ 2 ];


	//Scene Mesh Objects
	MeshFileHeader header;
	MeshData m_meshData;
	MeshObjectHandle m_MeshScene;

	// Camera objects 
	IFNITY::EventCameraListener m_CameraListener;
	CameraPositioner_FirstPerson m_camera;


public:
	Source(IFNITY::rhi::GraphicsAPI api):
		IFNITY::App(api),
		m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr()),
		m_camera(vec3(35.f, 0.5f, -10.f), vec3(-1.0f, -90, -1.0f), vec3(0.0f, 1.0f, 0.0f)),
		m_CameraListener(&m_camera)
	{
		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushLayer(new IFNITY::CameraLayer(&m_CameraListener));
		PushOverlay(new IFNITY::ImguiLayer());
		
		

	}

	void Initialize() override
	{


		m_ManagerDevice = &App::GetApp().GetManagerDevice();


		IFNITY::ShaderCompiler::Initialize();

		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);

		m_vs[ GRID ] = std::make_shared<IShader>();
		m_ps[ GRID ] = std::make_shared<IShader>();
		m_vs[ SCENE ] = std::make_shared<IShader>();
		m_ps[ SCENE ] = std::make_shared<IShader>();


		auto vSceneconfig = IFNITY::readSceneConfig("data/sceneconverter.json");
		auto* rdevice = m_ManagerDevice->GetRenderDevice();


		//Create Shaders Description 
		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "GL01_grid.vert";
			m_vs[ GRID ]->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "GL01_grid.frag";
			m_ps[ GRID ]->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "scene_wireframe.vs";
			m_vs[ SCENE ]->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "scene_wireframe.fs";
			m_ps[ SCENE ]->SetShaderDescription(DescriptionShader);

		}


		//Compiler Shaders 
		ShaderCompiler::CompileShader(m_vs[ GRID ].get());
		ShaderCompiler::CompileShader(m_ps[ GRID ].get());
		ShaderCompiler::CompileShader(m_vs[ SCENE ].get());
		ShaderCompiler::CompileShader(m_ps[ SCENE ].get());


		GraphicsPipelineDescription gdesc;
		BlendState blendstate;
		blendstate.setBlendEnable(true).
				   setSrcBlend(BlendFactor::SRC_ALPHA).
			       setDestBlend(BlendFactor::ONE_MINUS_SRC_ALPHA);

		gdesc.SetVertexShader(m_vs[ GRID ].get()).
			  SetPixelShader(m_ps[ GRID ].get()).
			  SetRenderState(RenderState{ .depthTest = true, .depthWrite = true , .blendState = blendstate });


		m_GraphicsPipeline[ GRID ] = rdevice->CreateGraphicsPipeline(gdesc);



		BufferDescription DescriptionBuffer;
		DescriptionBuffer.SetBufferType(BufferType::CONSTANT_BUFFER)
			.SetByteSize(sizeof(PerFrameData))
			.SetDebugName("UBO MVP PERFRAME DATA ")
			.SetStrideSize(0)
			.SetOffset(0)
			.SetBindingPoint(kBufferIndex_PerFrame);

		m_UBO = rdevice->CreateBuffer(DescriptionBuffer);













		MeshObjectDescription meshAssimp =
		{
			.filePath = vSceneconfig[3].fileName,
			.isLargeMesh = true,
			.isGeometryModel = false,
			.meshData = MeshData{},
			.meshFileHeader = MeshFileHeader{},
			.meshDataBuilder = nullptr,
			.sceneConfig = vSceneconfig[ 3 ]
		};

		//MeshDataBuilderAssimp<rhi::VertexScene> builder(8);

		//builder.buildSceneData(meshAssimp);

		//Create a SceneObject with the data.
		m_SceneObject = rdevice->CreateSceneObject(meshAssimp.sceneConfig.outputMesh.c_str(),
												   meshAssimp.sceneConfig.outputScene.c_str(),
												   meshAssimp.sceneConfig.outputMaterials.c_str());

		//Create the m_SceneObject with the device
		m_MeshObject = rdevice->CreateMeshObjectFromScene(m_SceneObject);




	}
	
	void Render() override
	{
			m_GraphicsPipeline[ GRID ]->BindPipeline(m_ManagerDevice->GetRenderDevice());

		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());
		IFNITY_LOG(LogApp, INFO, "Mesh loaded, ready to render");

		const mat4 p = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);
		const mat4 view = m_camera.getViewMatrix();

		const PerFrameData perFrameData = { .view = view, .proj = p, .cameraPos = glm::vec4(m_camera.getPosition(), 1.0f) };

		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, &perFrameData, sizeof(PerFrameData));

		//Draw Description 
		DrawDescription desc;
		ViewPortState descViewPort;
		descViewPort.width = m_ManagerDevice->GetWidth();
		descViewPort.height = m_ManagerDevice->GetHeight();
		descViewPort.minDepth = 0.0f;
		descViewPort.maxDepth = 1.0f;

		desc.SetViewPortState(descViewPort);
		//desc.rasterizationState.fillMode = FillModeType::Wireframe;
		desc.size = 6;  //Two triangles 
		desc.indices = nullptr;
		desc.isIndexed = false;

		m_ManagerDevice->GetRenderDevice()->Draw(desc);
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


	auto api = IFNITY::rhi::GraphicsAPI::OPENGL;


	//return new Source_TestD3D12(api);
	return new Source(api);
}

