// IFNITY.cp

#include <Ifnity.h>
#include <stb_image.h>

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
		if( eventBus )
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
			// Manage conversion pointer error. 
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
		if( context == nullptr )
		{
			IFNITY_LOG(LogApp, ERROR, "Failed to get ImGui context from DLL");
			return;
		}
		ImGui::SetCurrentContext(context);

		ChooseApi();
		//IFNITY_LOG(LogApp, INFO, "Update ImGuiTest Layer OnUpdate");
	}

	void ConnectToEventBusImpl(void* bus) override
	{}

private:
	// A function that is called when the button is clicked
	void AccionPorOpcion(int opcionSeleccionada)
	{
		GraphicsAPI api = IFNITY::App::GetApp().GetGraphicsAPI();
		switch( opcionSeleccionada )
		{
			case 0:
				IFNITY_LOG(LogApp, INFO, "OPENGL");
				IFNITY::App::GetApp()
					.SetGraphicsAPI(GraphicsAPI::OPENGL, api != GraphicsAPI::OPENGL);
				break;
			case 1:
				IFNITY_LOG(LogApp, INFO, "D3D11");
				IFNITY::App::GetApp()
					.SetGraphicsAPI(GraphicsAPI::D3D11, api != GraphicsAPI::D3D11);
				break;
			case 2:
				IFNITY_LOG(LogApp, INFO, "D3D12");
				IFNITY::App::GetApp()
					.SetGraphicsAPI(GraphicsAPI::D3D12, api != GraphicsAPI::D3D12);
				break;
			case 3:
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
		const char* options[] = { "OPENGL", "D3D11", "D3D12", "VULKAN" };

		ImGui::Begin("API WINDOW");

		// Combo box with the options
		if( ImGui::Combo("Choose Option ", &selectOption, options, IM_ARRAYSIZE(options)) )
		{
			// This block executes when a new option is selected
		}

		// Button that triggers the selected action
		if( ImGui::Button("OK") )
		{
			AccionPorOpcion(selectOption);
		}

		ImGui::End();
	}
};

class Source: public IFNITY::App
{

private:
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;

	//CAMERA LAYER
	// Camera objects 
	IFNITY::EventCameraListener m_CameraListener;
	CameraPositioner_FirstPerson m_camera;
	const vec3 kInitialCameraPos    = vec3(0.0f, 1.0f, -1.5f);
	const vec3 kInitialCameraTarget = vec3(0.0f, 0.5f, 0.0f);


	//FPS Counter
	IFNITY::FpsCounter m_FpsCounter;
	float deltaSeconds = 0.0f;
	double timeStamp = 0.0;


	struct VertexData
	{
		vec3 pos;
		vec3 normal;
		vec2 tc;
	};


	struct PerFrameData
	{
		mat4 model;
		mat4 view;
		mat4 proj;
		vec4 cameraPos;
		uint32_t tex = 0;
	};

	std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
	TextureHandle m_texture;
	BufferHandle m_vertexBuffer;
	BufferHandle m_indexBuffer;
	BufferHandle m_PushConnstant; // Uniform Buffer Object for per-frame data
	GraphicsPipelineHandle m_pipeline;
	GraphicsDeviceManager* m_ManagerDevice;
public:



	Source(IFNITY::rhi::GraphicsAPI api): 
		IFNITY::App(api),
		m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr()),
		m_camera(vec3(0.f, 1.0f, -1.5f), vec3(0.f, -0.5f, -0.0f), vec3(0.0f, 1.0f, 0.0f)),
		m_CameraListener(&m_camera)
	{
		// Push layers including monitoring and GUI
		PushLayer(new IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushLayer(new IFNITY::CameraLayer(&m_CameraListener));
		PushOverlay(new IFNITY::ImguiLayer()); // DLL-based ImGui overlay
	}

	//Vertex struct initialization 


	void Initialize() override
	{
		//File System Instance get it 
		auto& vfs = IFNITY::VFS::GetInstance();
		ShaderCompiler::Initialize();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);
		vfs.Mount("test", "Shaders/testShader", IFNITY::FolderType::NO_DEFINED);

		//auto files = vfs.ListFilesInCurrentDirectory("test");
		auto* rdevice = m_ManagerDevice->GetRenderDevice();

		IFNITY_LOG(LogApp, INFO, "START COMPILING INFO  ");

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();

		ShaderCreateDescription descShader;
		{
			descShader.NoCompile = false;
			descShader.FileName = "assimp.hlsl";
			descShader.EntryPoint = L"VSMain";
			descShader.Profile = L"vs_6_6";
			descShader.Type = ShaderType::VERTEX_SHADER;
			descShader.APIflag = ShaderAPIflag::ONLY_HLSL;
			descShader.Flags = ShaderCompileFlagType::ENABLE_DEBUG_INFO;
			m_vs->SetShaderDescription(descShader);
		}
		ShaderCompiler::CompileShader(m_vs.get());
		{
			descShader.NoCompile = false;
			descShader.EntryPoint = L"PSMain";
			descShader.Profile = L"ps_6_6";
			descShader.Type = ShaderType::PIXEL_SHADER;
			descShader.APIflag = ShaderAPIflag::ONLY_HLSL;
			descShader.Flags = ShaderCompileFlagType::ENABLE_DEBUG_INFO;
			m_ps->SetShaderDescription(descShader);
		}
		ShaderCompiler::CompileShader(m_ps.get());


		GraphicsPipelineDescription gdesc;
		{


			//Vertex Attributes Configure 
			rhi::VertexInput vertexInput;
			uint8_t position = 0;
			uint8_t normal = 1;
			uint8_t tc = 2;
			vertexInput.addVertexAttribute({ .semantic = rhi::VertexSemantic::POSITION, 
											.location = position,
											.binding = 0,
											.format = rhi::Format::R32G32B32_FLOAT,
											.offset = offsetof(VertexData,pos) }, position);

			vertexInput.addVertexAttribute({ .semantic = rhi::VertexSemantic::NORMAL,
											.location = normal,
											.binding = 0,
											.format = rhi::Format::R32G32B32_FLOAT,
											.offset = offsetof(VertexData,normal) }, normal);
			vertexInput.addVertexAttribute( { .semantic = rhi::VertexSemantic::TEXCOORD,
											.location = tc,
											.binding = 0,
											.format = rhi::Format::R32G32_FLOAT,
											.offset = offsetof( VertexData,tc ) }, tc );

			vertexInput.addVertexInputBinding({ .stride = sizeof(VertexData) }, position);




			gdesc.SetVertexShader(m_vs.get())
				.SetPixelShader(m_ps.get())
				.SetVertexInput(vertexInput);

			RasterizationState rasterizationState;
			rasterizationState.cullMode = rhi::CullModeType::FrontAndBack;




			gdesc.SetRasterizationState( rasterizationState );


		}//end of gdesc
		 //Create the pipeline
		m_pipeline = rdevice->CreateGraphicsPipeline(gdesc);


		//Scene assimp

		
		{
			//assimp 
			const aiScene* scene = aiImportFile("data/rubber_duck/scene.gltf", aiProcess_Triangulate);
			//const aiScene* scene = aiImportFile("data/helmet/helmet.gltf", aiProcess_Triangulate);

			if( !scene || !scene->HasMeshes() )
			{
				printf("Unable to load data/rubber_duck/scene.gltf\n");
				exit(255);
			}

			const aiMesh* mesh = scene->mMeshes[ 0 ];
			
			for( uint32_t i = 0; i != mesh->mNumVertices; i++ )
			{
				const aiVector3D v = mesh->mVertices[ i ];
				const aiVector3D n = mesh->mNormals[ i ];
				const aiVector3D t = mesh->mTextureCoords[ 0 ][ i ];
				vertices.push_back({ .pos = vec3(v.x, v.y, v.z),
									.normal = vec3(n.x, n.y, n.z),
									.tc = vec2(t.x, 1.0 - t.y) });
			}
			
			for( uint32_t i = 0; i != mesh->mNumFaces; i++ )
			{
				for( uint32_t j = 0; j != 3; j++ )
					indices.push_back(mesh->mFaces[ i ].mIndices[ j ]);
			}

			aiReleaseImport(scene);
		}

		const size_t kSizeIndices = sizeof(uint32_t) * indices.size();
		const size_t kSizeVertices = sizeof(VertexData) * vertices.size();


		BufferDescription bufferDesc;
		{
			bufferDesc.SetDebugName( "Vertex Buffer" );
			bufferDesc.SetBufferType( BufferType::VERTEX_BUFFER );
			bufferDesc.SetStorageType( StorageType::DEVICE );
			bufferDesc.SetByteSize( kSizeVertices);
			bufferDesc.SetData( vertices.data() );
			bufferDesc.SetStrideSize( sizeof( VertexData ) );

		}
		m_vertexBuffer = rdevice->CreateBuffer( bufferDesc );

		{
			bufferDesc.SetDebugName( "Index Buffer" );
			bufferDesc.SetBufferType( BufferType::INDEX_BUFFER );
			bufferDesc.SetStorageType( StorageType::HOST_VISIBLE );
			bufferDesc.SetByteSize( kSizeIndices );
			bufferDesc.SetData( indices.data());
			bufferDesc.SetStrideSize( sizeof( uint32_t ) );
		}
		m_indexBuffer = rdevice->CreateBuffer( bufferDesc );


		{
			bufferDesc.SetDebugName( "PushConstant Buffer" );
			bufferDesc.SetBufferType( BufferType::CONSTANT_BUFFER );
			bufferDesc.SetStorageType( StorageType::HOST_VISIBLE );
			bufferDesc.SetByteSize(  sizeof(PerFrameData) );
			bufferDesc.SetData( nullptr );

		}

		m_PushConnstant = rdevice->CreateBuffer(bufferDesc);

		{
			int w, h, comp;
			const uint8_t* img = stbi_load("data/rubber_duck/textures/Duck_baseColor.png", &w, &h, &comp, 4);

			if( !img )
			{
				IFNITY_LOG(LogApp, ERROR, "Failed to load image");
				assert(false);
			}


			TextureDescription descTexture;
			descTexture.dimensions = {(uint32_t)w, (uint32_t)h};
			descTexture.format = Format::RGBA_UNORM8;
			descTexture.usage = TextureUsageBits::SAMPLED;
			descTexture.isDepth = false;
			descTexture.debugName = "ImageAssimp";
			descTexture.data = img;

			//DepthStencil texture
			m_texture = rdevice->CreateTexture(descTexture);

			auto value = m_texture.get()->GetTextureID();
		}

		//Binding the buffer
		rdevice->BindingIndexBuffer( m_indexBuffer );
		rdevice->BindingVertexAttributesBuffer(m_vertexBuffer);

		//BindPipeline
		m_pipeline->BindPipeline( rdevice );



	}

	void Render() override
	{

		//Update FPS 
		const double newTimeStamp = App::GetTime();
		deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;
		m_FpsCounter.tick(deltaSeconds);




		auto* rdevice = m_ManagerDevice->GetRenderDevice();





		const vec4 cameraPos = vec4(m_camera.getPosition(), 1.0f);
		float ratio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());

		const mat4 p = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 1000.0f);
		const mat4 m1 = glm::rotate(mat4(1.0f), glm::radians(-90.0f), vec3(1, 0, 0));
		const mat4 m2 = glm::rotate(mat4(1.0f), (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
		const mat4 v = glm::translate(mat4(1.0f), vec3(cameraPos));


		

		/*const mat4 m = glm::rotate(mat4(1.0f), glm::radians(-90.0f), vec3(1, 0, 0));
		const mat4 v = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, -0.0f, -2.75f)), (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
		const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);*/
		glm::mat3 normalMatrix = glm::inverse(glm::mat3(m2*m1));
	 //clave

		const PerFrameData pc = {
			.model = m2*m1,
			.view = m_camera.getViewMatrix(),
			.proj = p,
			.cameraPos = cameraPos,
			.tex = m_texture->GetTextureID(),
			//.texCube   = cubemapTex.index(),
		};


		DrawDescription desc;
		desc.size = static_cast< uint32_t >(indices.size());
		desc.drawMode = DRAW_INDEXED;



		rdevice->StartRecording();
		rdevice->WriteBuffer(m_PushConnstant, &pc, sizeof(PerFrameData));


		rdevice->DrawObject( m_pipeline, desc );
		rdevice->StopRecording();





	}

	void Animate() override
	{

	}

	~Source() override {



	}
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
	auto api = IFNITY::rhi::GraphicsAPI::D3D12;

	//return new Source_TestD3D12(api);
	return new Source(api);
}

