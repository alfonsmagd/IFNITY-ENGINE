

// IFNITY.cp

#include <Ifnity.h>


using namespace IFNITY;
using namespace IFNITY::rhi;
using glm::vec3;
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



class ImGuiTestLayer : public IFNITY::Layer
{
public:
	ImGuiTestLayer() : Layer("ImGuiTest"), m_Device(&IFNITY::App::GetApp().GetManagerDevice()) {}
	~ImGuiTestLayer() {}

	void OnAttach() override
	{
		
		m_Device = &IFNITY::App::GetApp().GetManagerDevice();
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

		//ChooseApi();
		ShowColorPiceckerWindow();
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

	void ShowColorPiceckerWindow()
	{
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
		ImGui::Begin("Color Picker");
		static float clearColor[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
		// Selector de color
		if (ImGui::ColorEdit4("Clear Color", clearColor))
		{
			// Si el color cambia, actualiza el color del buffer de fondo
			m_Device->ClearBackBuffer(clearColor);
		}
		ImGui::End();
	}

	IFNITY::GraphicsDeviceManager* m_Device;
};


class Source: public IFNITY::App
{

public:

	Source(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr())
	{
		
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

static const float2 g_positions[] =
{
    float2(-0.5, -0.5),
    float2(0, 0.5),
    float2(0.5, -0.5)
};

static const float3 g_colors[] =
{
    float3(1, 1, 1),
    float3(0, 1, 0),
    float3(0, 0, 1)
};

void main_vs(
    uint i_vertexId : SV_VertexID,
    out float4 o_pos : SV_Position,
    out float3 o_color : COLOR
)
{
      o_pos = mul(float4(g_positions[i_vertexId], 0, 1),MVP);
    o_color = g_colors[i_vertexId];
}

void main_ps(
    in float4 i_pos : SV_Position,
    in float3 i_color : COLOR,
    out float4 o_color : SV_Target0
)
{
    
        o_color = float4(i_color, 1);
    
}
)";


		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);
		////Use filesystems to init 
		//std::vector<std::string> files = vfs.ListFiles("Shaders","vk");
		//for (const auto& file : files)
		//{
		//	std::cout << file << std::endl;
		//}
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
			 

	}

	void Render() override
	{
		using namespace math;
		//SetPipelineState
		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());

		const mat4 mg = glm::rotate(mat4(1.0f), (float)glfwGetTime(), vec3(0.0f, 0.0f, -1.0f));
		const mat4 fg = glm::ortho(-aspectRatio, aspectRatio, -1.f, 1.f, 1.f, -1.f);
		const mat4 mvpg = fg * mg;



		const float4x4 m = rotate(float4x4::identity(), (float)glfwGetTime(), float3(0.f, 0.f, 1.0f));
		const float4x4 p = orthoProjOGLStyle(-aspectRatio, aspectRatio, -1.0f, 1.0f, 1.0f, -1.0f);
		//
		const float4x4 mvpd = m*p ;

	
		
		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, glm::value_ptr(mvpg), sizeof(mvpg));
		//m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, mvpd.m_data, sizeof(mvpd));

		//Draw Description 
		DrawDescription desc;
		desc.size = 3;
		m_ManagerDevice->GetRenderDevice()->Draw(desc);

		IFNITY_LOG(LogApp, INFO, "Render App");
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}
	~Source() override {}

private:
	BufferHandle m_UBO;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipeline m_GraphicsPipeline;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;
};

//This is global 


void error_callback(void*, const char* error_message)
{
	// Handle the error message here  
	std::cerr << "Error: " << error_message << std::endl;
}

IFNITY::App* IFNITY::CreateApp()
{


	auto api = IFNITY::rhi::GraphicsAPI::OPENGL;

	
	return new Source(api);
}

