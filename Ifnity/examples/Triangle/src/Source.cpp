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
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;
	//FPS Counter
	IFNITY::FpsCounter m_FpsCounter;
	float deltaSeconds = 0.0f;
	double timeStamp = 0.0;

	BufferHandle m_vertexBuffer;
	BufferHandle m_indexBuffer;
	GraphicsPipelineHandle m_pipeline;
	GraphicsDeviceManager* m_ManagerDevice;

public:



	Source( IFNITY::rhi::GraphicsAPI api ): IFNITY::App( api ), m_ManagerDevice( IFNITY::App::GetApp().GetDevicePtr() )
	{
		// Push layers including monitoring and GUI
		PushLayer( new IFNITY::NVML_Monitor() );
		PushLayer( new ImGuiTestLayer() );
		PushOverlay( new IFNITY::ImguiLayer() ); // DLL-based ImGui overlay
	}

	//Vertex struct initialization 
	struct VertexData
	{
		vec3 pos;
		vec4 color;
	};

	void Initialize() override
	{
		//File System Instance get it 
		auto& vfs = IFNITY::VFS::GetInstance();
		ShaderCompiler::Initialize();

		vfs.Mount( "Shaders", "Shaders", IFNITY::FolderType::SHADERS );
		vfs.Mount( "test", "Shaders/testShader", IFNITY::FolderType::NO_DEFINED );

		//auto files = vfs.ListFilesInCurrentDirectory("test");
		auto* rdevice = m_ManagerDevice->GetRenderDevice();

		IFNITY_LOG( LogApp, INFO, "START COMPILING INFO  " );

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();


		const std::wstring shader = LR"(struct PSInput
		{
		    float4 position : SV_POSITION;
		    float4 color    : COLOR;
		};
		
		PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
		{
		    PSInput result;
		
		    result.position = position;
		    result.color = color;
		
		    return result;
		}
		
		float4 PSMain(PSInput input) : SV_TARGET
		{
		    return input.color;
		}
		)";

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.EntryPoint = L"VSMain";
			DescriptionShader.Profile = L"vs_6_6";
			DescriptionShader.Type = ShaderType::VERTEX_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = shader;
			DescriptionShader.FileName = "vsTriangle";
			DescriptionShader.APIflag = ShaderAPIflag::SPIRV_BIN;
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.EntryPoint = L"PSMain";
			DescriptionShader.Profile = L"ps_6_6";
			DescriptionShader.Type = ShaderType::PIXEL_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = shader;
			DescriptionShader.FileName = "psTriangle";
			DescriptionShader.APIflag = ShaderAPIflag::SPIRV_BIN;
			m_ps->SetShaderDescription(DescriptionShader);
		}

		ShaderCompiler::CompileShader(m_vs.get());
		ShaderCompiler::CompileShader(m_ps.get());

		GraphicsPipelineDescription gdesc;
		{


			//Vertex Attributes Configure 
			rhi::VertexInput vertexInput;
			uint8_t position = 0;
			uint8_t color = 1;
			vertexInput.addVertexAttribute( { .semantic = rhi::VertexSemantic::POSITION,
											.location = position,
											.binding = 0,
											.format = rhi::Format::R32G32B32_FLOAT,
											.offset = offsetof( VertexData,pos ) }, position );

			vertexInput.addVertexAttribute( { .semantic = rhi::VertexSemantic::COLOR,
											.location = color,
											.binding = 0,
											.format = rhi::Format::R32G32B32_FLOAT,
											.offset = offsetof( VertexData,color ) }, color );
			vertexInput.addVertexInputBinding( { .stride = sizeof( VertexData ) }, position );




			gdesc.SetVertexShader( m_vs.get() )
				.SetPixelShader( m_ps.get() )
				.SetVertexInput( vertexInput );

			RasterizationState rasterizationState;
			rasterizationState.cullMode = rhi::CullModeType::None;


			gdesc.SetRasterizationState( rasterizationState );


		}//end of gdesc
		 //Create the pipeline
		m_pipeline = rdevice->CreateGraphicsPipeline( gdesc );


		//Buffer Data 
		VertexData triangleVertices[] =
		{
			{ { -0.5f, -0.5f , 0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.0f, 0.5f , 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { 0.5f, -0.5f , 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
			{ { 0.5f, 0.5f , 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }

		};

		//IndexBuffer
		uint32_t indices[] = {
			0, 1, 2,  // triángulo inferior izquierdo
			2, 1, 3   // triángulo superior derecho
		};



		BufferDescription bufferDesc;
		{
			bufferDesc.SetDebugName( "Vertex Buffer" );
			bufferDesc.SetBufferType( BufferType::VERTEX_BUFFER );
			bufferDesc.SetStorageType( StorageType::DEVICE );
			bufferDesc.SetByteSize( sizeof( triangleVertices ) );
			bufferDesc.SetData( triangleVertices );
			bufferDesc.SetStrideSize( sizeof( VertexData ) );

		}
		m_vertexBuffer = rdevice->CreateBuffer( bufferDesc );

		{
			bufferDesc.SetDebugName( "Index Buffer" );
			bufferDesc.SetBufferType( BufferType::INDEX_BUFFER );
			bufferDesc.SetStorageType( StorageType::HOST_VISIBLE );
			bufferDesc.SetByteSize( sizeof( uint32_t ) * 6 );
			bufferDesc.SetData( indices );
			bufferDesc.SetStrideSize( sizeof( uint32_t ) );
		}
		m_indexBuffer = rdevice->CreateBuffer( bufferDesc );

		//BindPipeline
		m_pipeline->BindPipeline( rdevice );

		//Binding the buffer
		rdevice->BindingIndexBuffer( m_indexBuffer );
		rdevice->BindingVertexAttributesBuffer( m_vertexBuffer );



	}

	void Render() override
	{
		auto* rdevice = m_ManagerDevice->GetRenderDevice();

		//Update FPS
		const double newTimeStamp = App::GetTime();
		deltaSeconds = static_cast< float >(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;
		m_FpsCounter.tick( deltaSeconds );



		rdevice->StartRecording();

		DrawDescription desc;
		desc.drawMode = DRAW_INDEXED;
		desc.size = 3;

		rdevice->Draw(desc);

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

	//return new Source_TestD3D12(api);
	return new Source( api );
}

