

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



class ImGuiTestLayer: public IFNITY::Layer
{
public:
	DrawDescription descImgui;



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

		ChooseApi();
		EnableDisableDepthBias();
		//IFNITY_LOG(LogApp, INFO, "Update ImGuiTest Layer OnUpdate");
	}
	// Heredado v�a Layer
	void ConnectToEventBusImpl(void* bus) override
	{


	}
private:
	// Una funci�n que se llama al hacer clic en el bot�n
	void AccionPorOpcion(int opcionSeleccionada)
	{

		GraphicsAPI api = IFNITY::App::GetApp().GetGraphicsAPI();
		switch(opcionSeleccionada)
		{
		case 0:
			// Acci�n para la opci�n 1
			IFNITY_LOG(LogApp, INFO, "OPENGL");


			IFNITY::App::GetApp()
				.SetGraphicsAPI(GraphicsAPI::OPENGL, api != GraphicsAPI::OPENGL);
			break;
		case 1:
			// Acci�n para la opci�n 2
			IFNITY_LOG(LogApp, INFO, "D3D11");
			IFNITY::App::GetApp()
				.SetGraphicsAPI(GraphicsAPI::D3D11, api != GraphicsAPI::D3D11);
			break;

		case 2:
			// Acci�n para la opci�n 3
			IFNITY_LOG(LogApp, INFO, "D3D12");
			IFNITY::App::GetApp()
				.SetGraphicsAPI(GraphicsAPI::D3D12, api != GraphicsAPI::D3D12);
			break;

		case 3:
			// Acci�n para la opci�n 3
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

		ImGui::Begin("API WINDOW");  // Comienza la creaci�n de la ventana

		// Combo box con las opciones
		if(ImGui::Combo("Choose Option ", &selectOption, options, IM_ARRAYSIZE(options)))
		{
			// Este bloque se ejecuta cada vez que se selecciona una opci�n diferente
		}

		// Bot�n que ejecuta la funci�n cuando se hace clic
		if(ImGui::Button("OK"))
		{
			AccionPorOpcion(selectOption);
		}

		ImGui::End();  // Termina la creaci�n de la ventana
	}

	void EnableDisableDepthBias()
	{
		
		ImGui::Begin("Depth Bias");
		ImGui::Checkbox("Enable Depth Bias", &descImgui.enableBias);
		ImGui::Checkbox("Enable Depth Write Test", &descImgui.depthTest);
		ImGui::SliderFloat("Constant Factor", &descImgui.depthBiasValues.Constant, 0.0f, 10.0f);
		ImGui::SliderFloat("Clamp", &descImgui.depthBiasValues.Clamp, -10.0f, 5.0f);
		ImGui::SliderFloat("Slope Factor", &descImgui.depthBiasValues.Slope, 0.0f, 10.0f);
		ImGui::End();
	}

	




};

class Source: public IFNITY::App
{
private:
	BufferHandle m_UBO;
	BufferHandle m_vertexBuffer;
	BufferHandle m_indexBuffer;
	TextureHandle m_depth;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_SolidPipeline;
	GraphicsPipelineHandle m_WireFramePipeline;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;

	uint32_t msizeIndex = 0;	
	ImGuiTestLayer m_ImGuiLayer;
public:
	Source(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr())
	{
		// Obtener el contexto de ImGui desde IFNITY  DLL
		/*ImGuiContext* context = GetImGuiContext();
		if (context == nullptr)
		{
			IFNITY_LOG(LogApp, ERROR, "Failed to get ImGui context from DLL");
			return;
		}*/

		// Establecer el contexto de ImGui en la aplicaci�n principal
		//ImGui::SetCurrentContext(context);
		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(&m_ImGuiLayer);
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 



	}

	void Initialize() override
	{
		//File System Instance get it 
		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);
		vfs.Mount("test", "Shaders/testShader", IFNITY::FolderType::NO_DEFINED);

		//auto files = vfs.ListFilesInCurrentDirectory("test");
		auto* rdevice = m_ManagerDevice->GetRenderDevice();
		

		/*IFNITY::testShaderCompilation(getFileName(".vert"), "Shaders/testShader/main.vert.spv");
		IFNITY::testShaderCompilation(getFileName(".frag"), "Shaders/testShader/main.frag.spv");*/

		IFNITY_LOG(LogApp, INFO, "END APP ONLY TEST SHADER BUILDING ");



		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.NoCompile = true;
			//DescriptionShader.FileName = "triangle01.vert";
			DescriptionShader.FileName = "glm.vert";
			DescriptionShader.FileName = "position_wireframe.vert";
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			//DescriptionShader.FileName = "triangle01.frag";
			DescriptionShader.FileName = "glm.frag";
			DescriptionShader.FileName = "position_wireframe.frag";
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

		//Triangle Generation position only
		std::vector<vec3> triangleVertices = { 
			vec3(-0.5f, -0.5f, 0.0f), 
			vec3(0.6f, -0.5f, 0.5f),
			vec3(0.1f, 0.8f, 0.0f)
		};
		uint32_t vertexBufferSize = static_cast<uint32_t>(triangleVertices.size()) * sizeof(vec3);

		// Setup indices
		std::vector<uint32_t> indexBuffer{ 0, 1, 2 };
		uint32_t indexBufferSize = static_cast<uint32_t>(indexBuffer.size()) * sizeof(uint32_t);


		//assimp 
		//const aiScene* scene = aiImportFile("data/rubber_duck/scene.gltf", aiProcess_Triangulate);
		const aiScene* scene = aiImportFile("data/helmet/helmet.gltf", aiProcess_Triangulate);

		if(!scene || !scene->HasMeshes())
		{
			printf("Unable to load data/rubber_duck/scene.gltf\n");
			exit(255);
		}

		const aiMesh* mesh = scene->mMeshes[ 0 ];
		std::vector<vec3> positions;
		std::vector<uint32_t> indices;
		for(unsigned int i = 0; i != mesh->mNumVertices; i++)
		{
			const aiVector3D v = mesh->mVertices[ i ];
			positions.push_back(vec3(v.x, v.y, v.z));
		}

		for(unsigned int i = 0; i != mesh->mNumFaces; i++)
		{
			for(int j = 0; j != 3; j++)
			{
				indices.push_back(mesh->mFaces[ i ].mIndices[ j ]);
			}
		}



		aiReleaseImport(scene);


		//DepthText texture
		TextureDescription descTexture;
		descTexture.dimensions = { 1280, 720 ,1 };
		descTexture.format = Format::Z_FLOAT32;
		descTexture.usage = TextureUsageBits::ATTACHMENT;
		descTexture.isDepth = true;
		descTexture.debugName = "Depth buffer";
		descTexture.depthStencil = DepthStencilTextureFlags::DEPTH;

		//DepthStencil texture
		m_depth = rdevice->CreateTexture(descTexture);
		rdevice->SetDepthTexture(m_depth);
		
		//Vertex Attributes Configure Buffer 
		{
			desc.storage = StorageType::HOST_VISIBLE;
			desc.type = BufferType::VERTEX_BUFFER;
			desc.binding = 0;
			desc.size = vertexBufferSize;
			desc.size = sizeof(vec3) * positions.size();
			desc.data = positions.data();
			desc.debugName = "Buffer: vertex";

			m_vertexBuffer = rdevice->CreateBuffer(desc);
		}

		//Indexbuffer
		{
			//uint32_t indices[] = { 0, 1, 2 };
			desc.type = BufferType::INDEX_BUFFER;
			desc.size =  sizeof(uint32_t) * indices.size();
			desc.data = indices.data();
			desc.debugName = "Buffer: index";

			msizeIndex = indices.size();

			m_indexBuffer = rdevice->CreateBuffer(desc);

		}
		
		//Vertex Attributes Configure 
		rhi::VertexInput vertexInput;
		uint8_t position = 0;
		vertexInput.addVertexAttribute({ .location = position,
										  .binding = 0, 
										  .format = rhi::Format::R32G32B32_FLOAT,
										  .offset = 0 }, position);

		vertexInput.addVertexInputBinding({ .stride = sizeof(vec3) }, position);

		{
			gdesc1.SetVertexShader(m_vs.get()).
				SetPixelShader(m_ps.get()).
				AddDebugName("Wireframe Pipeline").
				SetVertexInput(vertexInput).
				SetRasterizationState({ .cullMode = rhi::CullModeType::Back ,.polygonMode = rhi::PolygonModeType::Line }).
				AddSpecializationConstant({ .id = 0, .size = sizeof(uint32_t) , .dataSize = sizeof(wireframe),.data = &wireframe , })
				.SetRenderState({ .depthTest = true, .depthFormat = Format::Z_FLOAT32 });

			//GraphicsPipelineDescription gdesc
			m_WireFramePipeline = rdevice->CreateGraphicsPipeline(gdesc1);
		}

		{
			gdesc2.SetVertexShader(m_vs.get()).
				SetPixelShader(m_ps.get()).
				AddDebugName("Solid Pipeline").
				SetVertexInput(vertexInput).
				SetRasterizationState({ .cullMode = rhi::CullModeType::Back ,.polygonMode = rhi::PolygonModeType::Fill })
				.SetRenderState({ .depthTest = true, .depthFormat = Format::Z_FLOAT32 });

			m_SolidPipeline = rdevice->CreateGraphicsPipeline(gdesc2);
		}

		rdevice->BindingIndexBuffer(m_indexBuffer);
		rdevice->BindingVertexAttributesBuffer(m_vertexBuffer);


		
		m_SolidPipeline->BindPipeline(rdevice);
		

		//exit(0);

	}

	void Render() override
	{
		auto* rdevice = m_ManagerDevice->GetRenderDevice();


		float ratio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());

		const mat4 m = glm::rotate(mat4(1.0f), glm::radians(90.0f), vec3(1, 0, 0));
		const mat4 v = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, -0.0f, -2.75f)), (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
		const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);

		//StartRecording
		rdevice->StartRecording();
		rdevice->WriteBuffer(m_UBO, glm::value_ptr(p * v*  m), sizeof(glm::mat4));
		DrawDescription desc;
		//First Draw solid line pipeline 
		{
			desc.size = msizeIndex;
			desc.drawMode = DRAW_INDEXED;
			desc.depthTest = m_ImGuiLayer.descImgui.depthTest;
			rdevice->DrawObject(m_SolidPipeline, desc);
		}
		//Second Draw wireframe pipeline
		{
			desc.enableBias = m_ImGuiLayer.descImgui.enableBias;
			desc.depthBiasValues = m_ImGuiLayer.descImgui.depthBiasValues;
			rdevice->DrawObject(m_WireFramePipeline, desc);
		}
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


	auto api = IFNITY::rhi::GraphicsAPI::VULKAN;


	//return new Source_TestD3D12(api);
	return new Source(api);
}

