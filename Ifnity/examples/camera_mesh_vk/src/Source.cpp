

// IFNITY.cp

#include <Ifnity.h>
#include "..\..\Ifnity\vendor\assimp\contrib\stb_image\stb_image.h"

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
		if( context == nullptr )
		{
			IFNITY_LOG(LogApp, ERROR, "Failed to get ImGui context from DLL");
			return;
		}
		ImGui::SetCurrentContext(context);

		ChooseApi();
		EnableDisableDepthBias();
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
		switch( opcionSeleccionada )
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
		if( ImGui::Combo("Choose Option ", &selectOption, options, IM_ARRAYSIZE(options)) )
		{
			// Este bloque se ejecuta cada vez que se selecciona una opción diferente
		}

		// Botón que ejecuta la función cuando se hace clic
		if( ImGui::Button("OK") )
		{
			AccionPorOpcion(selectOption);
		}

		ImGui::End();  // Termina la creación de la ventana
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
	BufferHandle m_bufferPerFrame;
	TextureHandle m_depth;
	TextureHandle texture;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_SolidPipeline;
	GraphicsPipelineHandle m_WireFramePipeline;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;

	//CAMERA LAYER
	// Camera objects 
	IFNITY::EventCameraListener m_CameraListener;
	CameraPositioner_FirstPerson m_camera;
	const vec3 kInitialCameraPos    = vec3(0.0f, 1.0f, -1.5f);
	const vec3 kInitialCameraTarget = vec3(0.0f, 0.5f, 0.0f);

	uint32_t msizeIndex = 0;
	ImGuiTestLayer* m_ImGuiLayer;
public:
	Source(IFNITY::rhi::GraphicsAPI api):
		IFNITY::App(api),
		m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr()),
		m_camera(vec3(0.f, 1.0f, -1.5f), vec3(0.f, -0.5f, -0.0f), vec3(0.0f, 1.0f, 0.0f)),
		m_CameraListener(&m_camera)
	{

		m_ImGuiLayer = new ImGuiTestLayer();
		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(m_ImGuiLayer);
		PushLayer(new IFNITY::CameraLayer(&m_CameraListener));
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 
	}

	//Vertex struct initialization 
	struct VertexData
	{
		vec3 pos;
		vec3 n;
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

	void Initialize() override
	{




		//File System Instance get it 
		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);
		vfs.Mount("test", "Shaders/testShader", IFNITY::FolderType::NO_DEFINED);

		auto* rdevice = m_ManagerDevice->GetRenderDevice();


		IFNITY_LOG(LogApp, INFO, "END APP ONLY TEST SHADER BUILDING ");

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.NoCompile = true;
			//DescriptionShader.FileName = "triangle01.vert";
			DescriptionShader.FileName = "glm.vert";
			DescriptionShader.FileName = "position_wireframe.vert";
			DescriptionShader.FileName = "mesh_camera.vert";
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			//DescriptionShader.FileName = "triangle01.frag";
			DescriptionShader.FileName = "glm.frag";
			DescriptionShader.FileName = "position_wireframe.frag";
			DescriptionShader.FileName = "mesh_camera.frag";
			m_ps->SetShaderDescription(DescriptionShader);
		}

		ShaderCompiler::CompileShader(m_vs.get());
		ShaderCompiler::CompileShader(m_ps.get());

		const uint32_t wireframe = 1;
		GraphicsPipelineDescription gdesc1;
		GraphicsPipelineDescription gdesc2;

		
	


		//assimp 
		//const aiScene* scene = aiImportFile("data/rubber_duck/scene.gltf", aiProcess_Triangulate);
		const aiScene* scene = aiImportFile("data/helmet/helmet.gltf", aiProcess_Triangulate);

		if( !scene || !scene->HasMeshes() )
		{
			printf("Unable to load data/rubber_duck/scene.gltf\n");
			exit(255);
		}

		const aiMesh* mesh = scene->mMeshes[ 0 ];
		std::vector<VertexData> vertices;
		for( uint32_t i = 0; i != mesh->mNumVertices; i++ )
		{
			const aiVector3D v = mesh->mVertices[ i ];
			const aiVector3D n = mesh->mNormals[ i ];
			const aiVector3D t = mesh->mTextureCoords[ 0 ][ i ];
			vertices.push_back({ .pos = vec3(v.x, v.y, v.z),
							   .n = vec3(n.x, n.y, n.z),
							   .tc = vec2(t.x, 1.0 - t.y) });
		}
		std::vector<uint32_t> indices;
		for( uint32_t i = 0; i != mesh->mNumFaces; i++ )
		{
			for( uint32_t j = 0; j != 3; j++ )
				indices.push_back(mesh->mFaces[ i ].mIndices[ j ]);
		}

		const size_t kSizeIndices = sizeof(uint32_t) * indices.size();
		const size_t kSizeVertices = sizeof(VertexData) * vertices.size();
		aiReleaseImport(scene);


		//DepthText texture
		TextureDescription descTexture;
		descTexture.dimensions = { 1280 , 720 ,1 };//please change this to the size of the window
		descTexture.format = Format::Z_FLOAT32;
		descTexture.usage = TextureUsageBits::ATTACHMENT;
		descTexture.isDepth = true;
		descTexture.debugName = "Depth buffer";
		descTexture.depthStencil = DepthStencilTextureFlags::DEPTH;

		//DepthStencil texture
		m_depth = rdevice->CreateTexture(descTexture);
		rdevice->SetDepthTexture(m_depth);



		//Texture simple 
		int w, h, comp;
		//const uint8_t* img = stbi_load("data/rubber_duck/textures/Duck_baseColor.png", &w, &h, &comp, 4);
		const uint8_t* img = stbi_load("data/helmet/Default_albedo.jpg", &w, &h, &comp, 4);

		//DepthText texture
		
		descTexture.dimensions = {(uint32_t)w, (uint32_t)h};
		descTexture.format = Format::RGBA_UNORM8;
		descTexture.usage = TextureUsageBits::SAMPLED;
		descTexture.isDepth = false;
		descTexture.debugName = "image";
		descTexture.data = img;

		//DepthStencil texture
		texture = rdevice->CreateTexture(descTexture);


		BufferDescription desc;
		//Vertex Attributes Configure Buffer 
		{

			desc.storage = StorageType::HOST_VISIBLE;
			desc.type = BufferType::VERTEX_BUFFER;
			desc.binding = 0;
			desc.byteSize = kSizeVertices;
			desc.data = vertices.data();
			desc.debugName = "Buffer: vertex";

			m_vertexBuffer = rdevice->CreateBuffer(desc);
		}
		//Indexbuffer
		{
			//uint32_t indices[] = { 0, 1, 2 };
			desc.type = BufferType::INDEX_BUFFER;
			desc.byteSize = kSizeIndices;
			desc.data = indices.data();
			desc.debugName = "Buffer: index";

			msizeIndex = indices.size();

			m_indexBuffer = rdevice->CreateBuffer(desc);

		}
		//BufferPerFrame 
		{

			desc.type = BufferType::UNIFORM_BUFFER;
			desc.byteSize = sizeof(PerFrameData);
			desc.debugName = "Buffer: per-frame";
			desc.data = nullptr;

			m_bufferPerFrame = rdevice->CreateBuffer(desc);
		}

		//Vertex Attributes Configure 
		rhi::VertexInput vertexInput;
		uint8_t position = 0;
		uint8_t normal = 1;
		uint8_t texcoord = 2;
		vertexInput.addVertexAttribute({ .location = position,
									   .binding = 0,
									   .format = rhi::Format::R32G32B32_FLOAT,
									   .offset = offsetof(VertexData,pos) }, position);

		vertexInput.addVertexAttribute({ .location = normal,
									   .binding = 0,
									   .format = rhi::Format::R32G32B32_FLOAT,
									   .offset = offsetof(VertexData,n) }, normal);

		vertexInput.addVertexAttribute({ .location = texcoord,
									   .binding = 0,
									   .format = rhi::Format::R32G32_FLOAT,
									   .offset = offsetof(VertexData,tc) }, texcoord);


		vertexInput.addVertexInputBinding({ .stride = sizeof(VertexData) }, position);


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



		const vec4 cameraPos = vec4(m_camera.getPosition(), 1.0f);

		const mat4 p = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 1000.0f);
		const mat4 m1 = glm::rotate(mat4(1.0f), glm::radians(90.0f), vec3(1, 0, 0));
		const mat4 m2 = glm::rotate(mat4(1.0f), (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
		const mat4 v = glm::translate(mat4(1.0f), vec3(cameraPos));


		/*const mat4 m = glm::rotate(mat4(1.0f), glm::radians(-90.0f), vec3(1, 0, 0));
		const mat4 v = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, -0.0f, -2.75f)), (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
		const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);*/




		const PerFrameData pc = {
			.model = m2*m1,
			.view = m_camera.getViewMatrix(),
			.proj = p,
			.cameraPos = cameraPos,
			.tex = texture->GetTextureID(),
			//.texCube   = cubemapTex.index(),
		};

		//StartRecording
		rdevice->StartRecording();
		rdevice->WriteBuffer(m_bufferPerFrame, &pc, sizeof(pc));
		DrawDescription desc;
		//First Draw solid line pipeline 
		{
			desc.size = msizeIndex;
			desc.drawMode = DRAW_INDEXED;
			desc.depthTest = m_ImGuiLayer->descImgui.depthTest;
			rdevice->DrawObject(m_SolidPipeline, desc);
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

