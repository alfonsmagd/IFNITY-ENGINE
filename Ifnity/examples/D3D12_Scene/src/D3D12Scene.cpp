// IFNITY.cp

#include <Ifnity.h>
#include <stb_image.h>

using namespace IFNITY;
using namespace IFNITY::rhi;

using vec3 = glm::vec3;

class ExampleLayer: public IFNITY::GLFWEventListener, public IFNITY::Layer
{
public:
	ExampleLayer(): Layer( "Example" ) {}
	~ExampleLayer() {}

	void OnUpdate() override
	{
		IFNITY_LOG( LogApp, INFO, "Update App" );
	}

	void onEventReceived( const IFNITY::WindowResize& event ) override
	{
		IFNITY_LOG( LogApp, WARNING, event.ToString() + "Example Layer" );
	}

	void ConnectToEventBusImpl( void* bus ) override
	{
		auto eventBus = static_cast< IFNITY::GLFWEventSource* >(bus);
		if( eventBus )
		{
			CONNECT_EVENT_LAYER( WindowResize, eventBus );
			CONNECT_EVENT_LAYER( WindowClose, eventBus );
			CONNECT_EVENT_LAYER( KeyPressed, eventBus );
			CONNECT_EVENT_LAYER( KeyRelease, eventBus );
			CONNECT_EVENT_LAYER( MouseMove, eventBus );
			CONNECT_EVENT_LAYER( ScrollMouseMove, eventBus );
			CONNECT_EVENT_LAYER( MouseClick, eventBus );
		} 

		else
		{
			// Manage conversion pointer error. 
			IFNITY_LOG( LogApp, ERROR, "The pointer is not type IFNITY::GLFWEventSource*" );
		}
	}
};

class NVMLLayer: public IFNITY::Layer
{
public:
	NVMLLayer(): Layer( "NVML" ) {}
	~NVMLLayer() {}

	void OnUpdate() override
	{
		monitor.refresh();
		monitor.display();
	}

	void ConnectToEventBusImpl( void* bus ) override
	{}

	void OnAttach() override
	{
		loggerDisplayMonitor = LoggerDisplayMonitor();
		monitor.setDisplay( &loggerDisplayMonitor );

		IFNITY_LOG( LogApp, INFO, "NVML Layer is attached" );
	}

private:
	NvmlMonitor monitor;
	LoggerDisplayMonitor loggerDisplayMonitor;
};

class ImGuiTestLayer: public IFNITY::Layer
{
public:
	ImGuiTestLayer(): Layer( "ImGuiTest" ) {}
	~ImGuiTestLayer() {}

	void OnAttach() override
	{
		IFNITY_LOG( LogApp, INFO, "ImGuiTest Layer is attached" );
	}

	void OnUpdate() override
	{
		ImGuiContext* context = GetImGuiContext();
		if( context == nullptr )
		{
			IFNITY_LOG( LogApp, ERROR, "Failed to get ImGui context from DLL" );
			return;
		}
		ImGui::SetCurrentContext( context );

		ChooseApi();
		//IFNITY_LOG(LogApp, INFO, "Update ImGuiTest Layer OnUpdate");
	}

	void ConnectToEventBusImpl( void* bus ) override
	{}

private:
	// A function that is called when the button is clicked
	void AccionPorOpcion( int opcionSeleccionada )
	{
		GraphicsAPI api = IFNITY::App::GetApp().GetGraphicsAPI();
		switch( opcionSeleccionada )
		{
			case 0:
				IFNITY_LOG( LogApp, INFO, "OPENGL" );
				IFNITY::App::GetApp()
					.SetGraphicsAPI( GraphicsAPI::OPENGL, api != GraphicsAPI::OPENGL );
				break;
			case 1:
				IFNITY_LOG( LogApp, INFO, "D3D11" );
				IFNITY::App::GetApp()
					.SetGraphicsAPI( GraphicsAPI::D3D11, api != GraphicsAPI::D3D11 );
				break;
			case 2:
				IFNITY_LOG( LogApp, INFO, "D3D12" );
				IFNITY::App::GetApp()
					.SetGraphicsAPI( GraphicsAPI::D3D12, api != GraphicsAPI::D3D12 );
				break;
			case 3:
				IFNITY_LOG( LogApp, INFO, "VULKAN" );
				IFNITY::App::GetApp()
					.SetGraphicsAPI( GraphicsAPI::VULKAN, api != GraphicsAPI::VULKAN );
				break;
			default:
				break;
		}
	}

	void ChooseApi()
	{
		static int selectOption = 0;
		const char* options[] = { "OPENGL", "D3D11", "D3D12", "VULKAN" };

		ImGui::Begin( "API WINDOW" );

		// Combo box with the options
		if( ImGui::Combo( "Choose Option ", &selectOption, options, IM_ARRAYSIZE( options ) ) )
		{
			// This block executes when a new option is selected
		}

		// Button that triggers the selected action
		if( ImGui::Button( "OK" ) )
		{
			AccionPorOpcion( selectOption );
		}

		ImGui::End();
	}
};

class Source: public IFNITY::App
{

private:
	SceneObjectHandler m_SceneObject;
	MeshObjectHandle m_MeshObject;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;

	//CAMERA LAYER
	// Camera objects 
	IFNITY::EventCameraListener m_CameraListener;
	CameraPositioner_FirstPerson m_camera;
	const vec3 kInitialCameraPos = vec3( 0.0f, 1.0f, -1.5f );
	const vec3 kInitialCameraTarget = vec3( 0.0f, 0.5f, 0.0f );
	//FPS Counter
	IFNITY::FpsCounter m_FpsCounter;
	float deltaSeconds = 0.0f;
	double timeStamp = 0.0;

	struct VertexData
	{
		vec3 pos;
		vec2 tc;
		vec3 normal;
	};

	struct PerFrameData
	{
		mat4 mvp;
	};

	BufferHandle m_PushConnstant; // Uniform Buffer Object for per-frame data
	GraphicsPipelineHandle m_pipeline;
	GraphicsDeviceManager* m_ManagerDevice;


public:



	Source( IFNITY::rhi::GraphicsAPI api ):
		IFNITY::App( api ),
		m_ManagerDevice( IFNITY::App::GetApp().GetDevicePtr() ),
		m_camera(vec3(-19.f, 18.0f, -6.5f), vec3(0.f, -0.5f, -0.0f), vec3(0.0f, 1.0f, 0.0f)),
		m_CameraListener( &m_camera )
	{
		// Push layers including monitoring and GUI
		PushLayer( new IFNITY::NVML_Monitor() );
		PushLayer( new ImGuiTestLayer() );
		PushLayer( new IFNITY::CameraLayer( &m_CameraListener ) );
		PushOverlay( new IFNITY::ImguiLayer() ); // DLL-based ImGui overlay
	}

	//Vertex struct initialization 


	void Initialize() override
	{
		//File System Instance get it 
		auto& vfs = IFNITY::VFS::GetInstance();
		ShaderCompiler::Initialize();

		vfs.Mount( "Shaders", "Shaders", IFNITY::FolderType::SHADERS );
		vfs.Mount( "test", "Shaders/testShader", IFNITY::FolderType::NO_DEFINED );


		auto vSceneconfig = IFNITY::readSceneConfig( "data/sceneconverter.json" );
		auto* rdevice = m_ManagerDevice->GetRenderDevice();

		#define BISTRO  0
		#define SPONZA  4
		#define SANMIGUEL 5

		MeshObjectDescription meshAssimp =
		{
			.filePath = vSceneconfig[ SPONZA ].fileName,
			.isLargeMesh = true,
			.isGeometryModel = false,
			.meshData = MeshData{},
			.meshFileHeader = MeshFileHeader{},
			.meshDataBuilder = nullptr,
			.sceneConfig = vSceneconfig[ SPONZA ]
		};

		//MeshDataBuilderAssimp<rhi::VertexScene> builder( 1 );

		//builder.buildSceneData( meshAssimp );

		//Create a SceneObject with the data.
		m_SceneObject = rdevice->CreateSceneObject( meshAssimp.sceneConfig.outputMesh.c_str(),
													meshAssimp.sceneConfig.outputScene.c_str(),
													meshAssimp.sceneConfig.outputMaterials.c_str() );

		//Create the m_SceneObject with the device
		m_MeshObject = rdevice->CreateMeshObjectFromScene( m_SceneObject );

		IFNITY_LOG( LogApp, INFO, "START COMPILING INFO  " );

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();



		ShaderCreateDescription descShader;
		{
			descShader.NoCompile = false;
			descShader.FileName = "Scene.hlsl";
			descShader.EntryPoint = L"VSMain";
			descShader.Profile = L"vs_6_6";
			descShader.Type = ShaderType::VERTEX_SHADER;
			descShader.APIflag = ShaderAPIflag::ONLY_HLSL;
			descShader.Flags = ShaderCompileFlagType::ENABLE_DEBUG_INFO;
			m_vs->SetShaderDescription( descShader );
		}
		ShaderCompiler::CompileShader( m_vs.get() );
		{
			descShader.NoCompile = false;
			descShader.EntryPoint = L"PSMain";
			descShader.Profile = L"ps_6_6";
			descShader.Type = ShaderType::PIXEL_SHADER;	
			descShader.APIflag = ShaderAPIflag::ONLY_HLSL;
			descShader.Flags = ShaderCompileFlagType::ENABLE_DEBUG_INFO;
			m_ps->SetShaderDescription( descShader );
		}
		ShaderCompiler::CompileShader( m_ps.get() );


		GraphicsPipelineDescription gdesc;
		{
			//Vertex Attributes Configure 
			rhi::VertexInput vertexInput;
			uint8_t position = 0;
			uint8_t normal = 2;
			uint8_t tc = 1;
			vertexInput.addVertexAttribute({ .semantic = rhi::VertexSemantic::POSITION, 
											.location = position,
											.binding = 0,
											.format = rhi::Format::R32G32B32_FLOAT,
											.offset = offsetof(VertexData,pos) }, position);
			vertexInput.addVertexAttribute( { .semantic = rhi::VertexSemantic::TEXCOORD,
											.location = tc,
											.binding = 0,
											.format = rhi::Format::R32G32_FLOAT,
											.offset = offsetof( VertexData,tc ) }, tc );

			vertexInput.addVertexAttribute({ .semantic = rhi::VertexSemantic::NORMAL,
											.location = normal,
											.binding = 0,
											.format = rhi::Format::R32G32B32_FLOAT,
											.offset = offsetof(VertexData,normal) }, normal);



			vertexInput.addVertexInputBinding({ .stride = sizeof(VertexData) }, position);




			gdesc.SetVertexShader(m_vs.get())
				.SetPixelShader(m_ps.get())
				.AddDebugName("Solid Pipeline")
				.SetVertexInput(vertexInput)
				.SetRasterizationState({ .cullMode = rhi::CullModeType::None ,.polygonMode = rhi::PolygonModeType::Fill })
				.SetRenderState({ .depthTest = true, .depthFormat = Format::Z_FLOAT32 });

			
			m_pipeline = rdevice->CreateGraphicsPipeline( gdesc );


			BufferDescription bufferDesc;
			{
				bufferDesc.SetDebugName( "PushConstant Buffer" );
				bufferDesc.SetBufferType( BufferType::CONSTANT_BUFFER );
				bufferDesc.SetStorageType( StorageType::HOST_VISIBLE );
				bufferDesc.SetByteSize( sizeof( PerFrameData ) );
				bufferDesc.SetData( nullptr );

			}
			m_PushConnstant = rdevice->CreateBuffer( bufferDesc );

			//BindPipeline
			m_pipeline->BindPipeline( rdevice );


		}

	}
	void Render() override
	{
		//Update FPS 
		const double newTimeStamp = App::GetTime();
		deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;
		m_FpsCounter.tick(deltaSeconds);

		//Get Rdevice and ratio image. 
		auto* rdevice = m_ManagerDevice->GetRenderDevice();
		float ratio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());

		//Update camera values
		const mat4 m(glm::scale(mat4(1.0f), vec3(1.f)));
		const mat4 p = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 1000.0f);
		const mat4 mvp = p * m_camera.getViewMatrix() * m;

		////StartRecording
		rdevice->StartRecording();

			rdevice->WriteBuffer(m_PushConnstant, &mvp, sizeof(mvp));
			m_MeshObject->DrawIndirect();

		rdevice->StopRecording();
	}

	void Animate() override
	{

	}

	~Source() override
	{



	}
};

class Source_TestD3D12: public IFNITY::App
{
public:
	Source_TestD3D12( IFNITY::rhi::GraphicsAPI api ): IFNITY::App( api )
	{
		PushLayer( new ExampleLayer() );
	}
	~Source_TestD3D12() override
	{}
};

IFNITY::App* IFNITY::CreateApp()
{
	auto api = IFNITY::rhi::GraphicsAPI::D3D12;

	return new Source( api );
}

