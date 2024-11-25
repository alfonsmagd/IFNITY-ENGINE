

// IFNITY.cp

#include <Ifnity.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>


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



class ImGuiTestLayer: public IFNITY::Layer
{
public:
	ImGuiTestLayer(): Layer("ImGuiTest"), m_Device(&IFNITY::App::GetApp().GetManagerDevice()) {}
	~ImGuiTestLayer() {}

	void OnAttach() override
	{

		m_Device = &IFNITY::App::GetApp().GetManagerDevice();
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
		ShowColorPiceckerWindow();
		ShowFillModeSelector();
		SetThickenessImgui();
		//IFNITY_LOG(LogApp, INFO, "Update ImGuiTest Layer OnUpdate");
	}
	// Heredado vía Layer
	void ConnectToEventBusImpl(void* bus) override
	{}

	// Método para obtener el valor de thickness
	float GetThickness() const { return m_Thickness; }
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

	void ShowColorPiceckerWindow()
	{
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
		ImGui::Begin("Color Picker");
		static float clearColor[ 4 ] = { 0.45f, 0.55f, 0.60f, 1.00f };
		// Selector de color
		if(ImGui::ColorEdit4("Clear Color", clearColor))
		{
			// Si el color cambia, actualiza el color del buffer de fondo
			m_Device->ClearBackBuffer(clearColor);
		}
		ImGui::End();
	}

	void SetThickenessImgui()
	{
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
		ImGui::Begin("Thickness");
		ImGui::SliderFloat("Thickness", &m_Thickness, 0.0f, 10.0f);
		ImGui::End();

	}

	void ShowFillModeSelector()
	{
		if(ImGui::Begin("Fill Mode Selector"))
		{
			const char* fillModeItems[] = { "Point", "Wireframe", "Solid" };
			static int currentItem = 1; // Inicialmente Wireframe

			if(ImGui::Combo("Fill Mode", &currentItem, fillModeItems, IM_ARRAYSIZE(fillModeItems)))
			{
				switch(currentItem)
				{
				case 0:
					currentFillMode = FillModeType::Point;
					break;
				case 1:
					currentFillMode = FillModeType::Wireframe;
					break;
				case 2:
					currentFillMode = FillModeType::Solid;
					break;
				}

				// Actualizar el estado de rasterización
				DrawDescription desc;
				desc.rasterizationState.fillMode = currentFillMode;
				desc.size = 3;
				m_Device->GetRenderDevice()->Draw(desc);
			}
		}
		ImGui::End();
	}
	FillModeType currentFillMode = FillModeType::Wireframe; // Valor inicial
	IFNITY::GraphicsDeviceManager* m_Device;
	float m_Thickness = 1.0f;
};



//------------------------------------------------- //
//  SOURCE_CUBEMAP_FIGURE                           //
//------------------------------------------------- //

class Source_CUBEMAP_FIGURE: public IFNITY::App
{

public:

	struct PerFrameData
	{
		mat4 model;
		mat4 mvp;
		vec4 cameraPos;
	};

	Source_CUBEMAP_FIGURE(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr()),
		positioner(vec3(0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f)),
		m_CameraListener(&positioner)
	{

		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushLayer(new IFNITY::CameraLayer(&m_CameraListener));
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 


	}

	void Initialize() override
	{
		m_ManagerDevice = &App::GetApp().GetManagerDevice();


		IFNITY::ShaderCompiler::Initialize();
		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "GL03_cube.vert";
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "GL03_cube.frag";
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
			.SetByteSize(sizeof(PerFrameData))
			.SetDebugName("UBO MVP")
			.SetStrideSize(0);

		m_UBO = m_ManagerDevice->GetRenderDevice()->CreateBuffer(DescriptionBuffer);

		TextureDescription descCubeTexture;
		descCubeTexture.setDimension(TextureDimension::TEXTURECUBE)
			.setFilePath("data/cube_boloni.hdr")
			.setWrapping(TextureWrapping::CLAMP_TO_EDGE);

		m_Texture = m_ManagerDevice->GetRenderDevice()->CreateTexture(descCubeTexture);



	}



	void Render() override
	{
		
		const double newTimeStamp = glfwGetTime();
		deltaSeconds = static_cast<float>(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;
		m_FpsCounter.tick(deltaSeconds);


		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());


		const mat4 m = glm::scale(mat4(1.0f), vec3(2.0f));
		const mat4 p = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);

		const PerFrameData perFrameData
		{
			m, //model
			p * positioner.getViewMatrix() * m, //mvp
			vec4(-1.0,-1.0,10.0,1.0f) //cameraPos
		};




		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, &perFrameData, sizeof(PerFrameData));

		//Draw Description 
		DrawDescription desc;

		desc.size = 36; // Cube vertex 
		desc.indices = (const void*)(0);
		desc.isIndexed = false;
		desc.viewPortState = ViewPortState(0, 0, m_ManagerDevice->GetWidth(), m_ManagerDevice->GetHeight());

		m_ManagerDevice->GetRenderDevice()->Draw(desc);



		IFNITY_LOG(LogApp, INFO, "Render App");
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}

	void LoadAssimp()
	{

	}
	~Source_CUBEMAP_FIGURE() override {}

private:

	TextureHandle m_Texture;
	BufferHandle m_UBO;
	BufferHandle m_VertexBuffer;
	BufferHandle m_IndexBuffer;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_GraphicsPipeline;
	unsigned int m_IndexCount;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;
	std::shared_ptr<IShader> m_gs;

	// Camera objects 
	IFNITY::EventCameraListener m_CameraListener;
	CameraPositioner_FirstPerson positioner;

	//FPS Counter
	IFNITY::FpsCounter m_FpsCounter;
	float deltaSeconds = 0.0f;
	double timeStamp = 0.0;
};

void error_callback(void*, const char* error_message)
{
	// Handle the error message here  
	std::cerr << "Error: " << error_message << std::endl;
}

IFNITY::App* IFNITY::CreateApp()
{


	auto api = IFNITY::rhi::GraphicsAPI::OPENGL;

	return new Source_CUBEMAP_FIGURE(api);
}

