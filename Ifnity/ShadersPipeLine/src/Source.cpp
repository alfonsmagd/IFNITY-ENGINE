

// IFNITY.cp

#include <Ifnity.h>


using namespace IFNITY;
using namespace IFNITY::rhi;

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
	ImGuiTestLayer() : Layer("ImGuiTest"), m_Device(&IFNITY::App::GetApp().GetDevice()) {}
	~ImGuiTestLayer() {}

	void OnAttach() override
	{
		
		m_Device = &IFNITY::App::GetApp().GetDevice();
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

	Source(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_Device(IFNITY::App::GetApp().GetDevicePtr())
	{
		
		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 

		
	}

	void Initialize() override
	{
		IFNITY::ShaderCompiler::Initialize();
		std::wstring shaderSource3 = LR"(
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
			o_pos = float4(g_positions[i_vertexId], 0, 1);
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


		// Almacenar los resultados de get() en variables temporales
		App::GetApp().GetDevice().LoadAppShaders(m_vs.get(), m_ps.get());

		auto* var = App::GetApp().GetDevice().GetRenderDevice();


		
	}

	void Render() override
	{
		IFNITY_LOG(LogApp, INFO, "Render App");
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}
	~Source() override {}

private:
	GraphicsDeviceManager* m_Device;
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

	

	 //Código HLSL para un simple pixel shader que devuelve color rojo
	/*std::wstring shaderSource = LR"(
		float4 main() : SV_Target {
		    return float4(1.0, 0.0, 0.0, 1.0);
		}
	)";*/

//	std::wstring shaderSource5 = LR"(
//cbuffer UBO : register(b0)
//{
//    float4x4 projectionMatrix;
//    float4x4 viewMatrix;
//    float4x4 modelMatrix;
//};
//
//struct VSInput
//{
//    float3 inPos : POSITION;
//    float3 inColor : COLOR; // Asegúrate de que este tipo sea float3
//};
//
//struct PSInput
//{
//    float3 outColor : COLOR; // Asegúrate de que este tipo sea float3
//    float4 gl_Position : SV_POSITION;
//};
//
//PSInput main(VSInput input)
//{
//    PSInput output;
//
//    output.outColor = input.inColor; // Asignar color de entrada
//	output.gl_Position =  mul(mul(mul(float4(input.inPos, 1.0), modelMatrix), viewMatrix), projectionMatrix);
//                         
//
//    return output;
//}
//
//	)";
//
//	std::wstring shaderSource6 = LR"(
//struct PSInput
//{
//    float3 outColor : COLOR; // Asegúrate de que este tipo sea float3
//};
//
//float4 main(PSInput input) : SV_TARGET
//{
//    return float4(input.outColor, 1.0); // Color con alpha = 1.0
//}
//
//	)";
//
//			std::wstring shaderSource2 = LR"(
//struct PS_INPUT
//		{
//			float4 pos : SV_POSITION;
//			float3 color : COLOR;
//		};
//
//		float4 main(PS_INPUT input) : SV_Target
//		{
//			return float4(input.color, 1.0); // Convertimos el color de 3 componentes a 4 componentes con alpha = 1.0
//		}
//	)";
//
//
//
//		std::wstring shaderSource = LR"(
//static const float2 _31[3] = { float2(-0.5, 0.0), float2(0.0, 0.5), float2(0.5, -0.5) };
//static const float3 _35[3] = { float3(1.0, 0.0, 0.0), float3(0.0, 1.0, 0.0), float3(0.0, 0.0, 1.0) };
//
//struct VS_OUTPUT
//{
//    float4 pos : SV_POSITION;
//    float3 color : COLOR;
//};
//
//VS_OUTPUT main(uint id : SV_VertexID)
//{
//    VS_OUTPUT output;
//    output.pos = float4(_31[id], 0.0, 1.0);
//    output.color = _35[id];
//    return output;
//})";

		


		return new Source(api);
}

