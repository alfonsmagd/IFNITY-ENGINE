

// IFNITY.cp

#include <Ifnity.h>
#include <spirv_cross_c.h>


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
	ImGuiTestLayer() : Layer("ImGuiTest"), m_Device(&IFNITY::App::GetApp().GetWindow()) {}
	~ImGuiTestLayer() {}

	void OnAttach() override
	{
		
		m_Device = &IFNITY::App::GetApp().GetWindow();
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
	Source(IFNITY::rhi::GraphicsAPI api) : IFNITY::App(api)
	{
		
		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 
		
		

	}
	~Source() override {}
};


std::vector<uint32_t> load_spirv_file(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		throw std::runtime_error("No se pudo abrir el archivo SPIR-V.");
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size % 4 != 0)
	{
		throw std::runtime_error("El tamaño del archivo SPIR-V no es múltiplo de 4.");
	}

	std::vector<uint32_t> buffer(size / 4);
	if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
	{
		throw std::runtime_error("Error al leer el archivo SPIR-V.");
	}

	return buffer;
}

void error_callback(void*, const char* error_message)
{
	// Handle the error message here  
	std::cerr << "Error: " << error_message << std::endl;
}

IFNITY::App* IFNITY::CreateApp()
{


	auto api = IFNITY::rhi::GraphicsAPI::OPENGL;

	IFNITY::ShaderCompiler::Initialize();

	 //Código HLSL para un simple pixel shader que devuelve color rojo
	/*std::wstring shaderSource = LR"(
		float4 main() : SV_Target {
		    return float4(1.0, 0.0, 0.0, 1.0);
		}
	)";*/

//	std::wstring shaderSource = LR"(
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
//	std::wstring shaderSource2 = LR"(
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

			std::wstring shaderSource2 = LR"(
struct PS_INPUT
		{
			float4 pos : SV_POSITION;
			float3 color : COLOR;
		};

		float4 main(PS_INPUT input) : SV_Target
		{
			return float4(input.color, 1.0); // Convertimos el color de 3 componentes a 4 componentes con alpha = 1.0
		}
	)";



		std::wstring shaderSource = LR"(
static const float2 _31[3] = { float2(-0.5, 0.0), float2(0.0, 0.5), float2(0.5, -0.5) };
static const float3 _35[3] = { float3(1.0, 0.0, 0.0), float3(0.0, 1.0, 0.0), float3(0.0, 0.0, 1.0) };

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

VS_OUTPUT main(uint id : SV_VertexID)
{
    VS_OUTPUT output;
    output.pos = float4(_31[id], 0.0, 1.0);
    output.color = _35[id];
    return output;
})";

		std::wstring shaderSource3 = LR"(
		static const float2 g_positions[] =
		{
			float2(-0.5, -0.5),
				float2(0, 0.5),
				float2(0.5, -0.5)
		};

		static const float3 g_colors[] =
		{
			float3(1, 0, 0),
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
		
	const std::wstring entryPoint = L"main_vs";
	const std::wstring entryPoint2 = L"main_ps";

	const std::wstring profile = L"vs_6_0"; // Perfil del shader, por ejemplo, pixel shader 6.0
	const std::wstring profile2 = L"ps_6_0"; // Perfil del shader, por ejemplo, pixel shader 6.0
	
	std::vector<uint32_t> spirv;
	
	ComPtr<IDxcBlob> blob1 = nullptr;
    HRESULT hr = ShaderCompiler::CompileShader(shaderSource3, entryPoint, profile,&blob1,"vsSimple.spv");
	ComPtr<IDxcBlob> blob2 = nullptr;
	hr = ShaderCompiler::CompileShader(shaderSource3, entryPoint2, profile2, &blob1, "psSimple.spv");


	// Leer SPIR-V desde el disco
	std::vector<uint32_t> spirv_binary = load_spirv_file("vsSimple.spv");

	spvc_context context = NULL;
	spvc_parsed_ir ir = NULL;
	spvc_compiler compiler_glsl = NULL;
	spvc_compiler_options options = NULL;
	spvc_resources resources = NULL;
	const spvc_reflected_resource* list = NULL;
	const char* result = NULL;
	size_t count;
	size_t i;

	// Crear contexto
	spvc_context_create(&context);
	
	// Establecer callback de error
	spvc_context_set_error_callback(context, error_callback, nullptr);
   


	// Parsear el SPIR-V
	spvc_context_parse_spirv(context, spirv_binary.data(), spirv_binary.size(), &ir);

	// Crear una instancia del compilador y darle propiedad del IR
	spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler_glsl);

	// Realizar reflexión básica
	spvc_compiler_create_shader_resources(compiler_glsl, &resources);
	spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &list, &count);

	for (i = 0; i < count; i++)
	{
		printf("ID: %u, BaseTypeID: %u, TypeID: %u, Name: %s\n", list[i].id, list[i].base_type_id, list[i].type_id,
			list[i].name);
		printf("  Set: %u, Binding: %u\n",
			spvc_compiler_get_decoration(compiler_glsl, list[i].id, SpvDecorationDescriptorSet),
			spvc_compiler_get_decoration(compiler_glsl, list[i].id, SpvDecorationBinding));
	}

	// Modificar opciones
	spvc_compiler_create_compiler_options(compiler_glsl, &options);
	spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_GLSL_VERSION, 450);
	spvc_compiler_install_compiler_options(compiler_glsl, options);

	// Compilar a GLSL
	spvc_compiler_compile(compiler_glsl, &result);
	printf("Cross-compiled source: %s\n", result);

	// Guardar la salida en un fichero
	std::ofstream outFile("output_sample.glsl");
	if (outFile.is_open())
	{
		outFile << result;
		outFile.close();
	}
	else
	{
		std::cerr << "No se pudo abrir el archivo para escribir." << std::endl;
	}


	// Liberar toda la memoria asignada hasta ahora
	spvc_context_destroy(context);

	
	//return new Source_TestD3D12(api);
	return new Source(api);
}

