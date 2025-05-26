

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
		//IFNITY_LOG(LogApp, INFO, "Update App");
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
	SceneObjectHandler m_SceneObject;
	MeshObjectHandle m_MeshObject;


	BufferHandle m_ConstantBuffer;
	BufferHandle m_vertexBuffer;
	BufferHandle m_indexBuffer;
	BufferHandle m_bufferPerFrame;
	TextureHandle m_depth;
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

	//FPS Counter
	IFNITY::FpsCounter m_FpsCounter;
	float deltaSeconds = 0.0f;
	double timeStamp = 0.0;



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
		vec2 tc;
		vec3 n;
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

		auto vSceneconfig = IFNITY::readSceneConfig("data/sceneconverter.json");
		auto* rdevice = m_ManagerDevice->GetRenderDevice();


		IFNITY_LOG(LogApp, INFO, "END APP ONLY TEST SHADER BUILDING ");

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "scene_vk.vert";
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "scene_vk.frag";
			m_ps->SetShaderDescription(DescriptionShader);
		}
		

		ShaderCompiler::CompileShader(m_vs.get());
		ShaderCompiler::CompileShader(m_ps.get());
	




		MeshObjectDescription meshAssimp =
		{
			.filePath = vSceneconfig[ 0 ].fileName,
			.isLargeMesh = true,
			.isGeometryModel = false,
			.meshData = MeshData{},
			.meshFileHeader = MeshFileHeader{},  
			.meshDataBuilder = nullptr,
			.sceneConfig = vSceneconfig[ 0 ]
		};

		//meshAssimp.meshFileHeader = loadMeshData("data/bistro/Exterior/exterior.obj.meshdata", meshAssimp.meshData);


		MeshDataBuilderAssimp<rhi::VertexScene> builder(1);

		//builder.buildSceneData(meshAssimp);

		//Create a SceneObject with the data.
		m_SceneObject = rdevice->CreateSceneObject(meshAssimp.sceneConfig.outputMesh.c_str(),
												   meshAssimp.sceneConfig.outputScene.c_str(),
												   meshAssimp.sceneConfig.outputMaterials.c_str());

		//Create the m_SceneObject with the device
		m_MeshObject = rdevice->CreateMeshObjectFromScene(m_SceneObject);


		//Buffer Consants 
		BufferDescription desc;
		desc.byteSize = sizeof(mat4);
		desc.type = BufferType::CONSTANT_BUFFER;
		desc.binding = 0;
		desc.data = nullptr;

		m_ConstantBuffer = rdevice->CreateBuffer(desc);




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


		//Vertex Attributes Configure 
		rhi::VertexInput vertexInput;
		uint8_t position = 0;
		uint8_t texcoord = 1;
		uint8_t normal = 2;
		vertexInput.addVertexAttribute({ .location = position,
									   .binding = 0,
									   .format = rhi::Format::R32G32B32_FLOAT,
									   .offset = offsetof(VertexData,pos) }, position);

		vertexInput.addVertexAttribute({ .location = texcoord,
									   .binding = 0,
									   .format = rhi::Format::R32G32_FLOAT,
									   .offset = offsetof(VertexData,tc) },texcoord);

		vertexInput.addVertexAttribute({ .location = normal,
									   .binding = 0,
									   .format = rhi::Format::R32G32B32_FLOAT,
									   .offset = offsetof(VertexData,n) }, normal);
		vertexInput.addVertexInputBinding({ .stride = sizeof(VertexData) }, position);



		GraphicsPipelineDescription gdesc2;
		{
			gdesc2.SetVertexShader(m_vs.get())
				.SetPixelShader(m_ps.get())
				.AddDebugName("Solid Pipeline")
				.SetVertexInput(vertexInput)
				.SetRasterizationState({ .cullMode = rhi::CullModeType::None ,.polygonMode = rhi::PolygonModeType::Fill })
				.SetRenderState({ .depthTest = true, .depthFormat = Format::Z_FLOAT32 });

			m_SolidPipeline = rdevice->CreateGraphicsPipeline(gdesc2);
		}

		m_SolidPipeline->BindPipeline(rdevice);



	}

	void Render() override
	{
		//Update FPS 
		const double newTimeStamp = App::GetTime();
		deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;
		m_FpsCounter.tick(deltaSeconds);


		auto* rdevice = m_ManagerDevice->GetRenderDevice();
		float ratio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());


		const mat4 m(glm::scale(mat4(1.0f), vec3(0.2f)));
		const mat4 p = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 1000.0f);
		const mat4 mvp = p * m_camera.getViewMatrix() * m;


		////StartRecording
		rdevice->StartRecording();
		rdevice->WriteBuffer(m_ConstantBuffer, &mvp, sizeof(mvp));

		m_MeshObject->DrawIndirect();


		rdevice->StopRecording();
		

		IFNITY_LOG(LogApp, INFO, "Render App");
	}

	void Animate() override
	{
		//IFNITY_LOG(LogApp, INFO, "Animate App");
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

