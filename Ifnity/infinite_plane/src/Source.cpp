

// IFNITY.cp

#include <Ifnity.h>

using namespace IFNITY;
using namespace IFNITY::rhi;
using glm::vec3;
using glm::vec4;
using glm::vec2;
using glm::mat4;

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



class ImGuiTestLayer : public IFNITY::Layer
{
public:
	ImGuiTestLayer() : Layer("ImGuiTest") {}
	~ImGuiTestLayer() {}

	void OnAttach() override
	{
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






	
};

class Source: public IFNITY::App
{

	struct PerFrameData
	{
		mat4 view;
		mat4 proj;
		vec4 cameraPos;
	};

public:
	Source(IFNITY::rhi::GraphicsAPI api):
		IFNITY::App(api),
		m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr()),
		m_camera(vec3(-0.5f, 0.5f, -0.f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f)),
		m_CameraListener(&m_camera)
	{

		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushLayer(new IFNITY::CameraLayer(&m_CameraListener));
		PushOverlay(new IFNITY::ImguiLayer());

		

	}


	void Initialize() override
	{
		m_ManagerDevice = &App::GetApp().GetManagerDevice();


		IFNITY::ShaderCompiler::Initialize();

		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();

		std::wstring infinite_grid_shader = LR"(
///////////////////////////////////////////
////////////////MAIN VS////////////////////
///////////////////////////////////////////
    cbuffer PerFrameData : register(b0) {
    float4x4 view;
    float4x4 proj;
    float4 cameraPos;
};

struct VertexInput {
    uint vertexID : SV_VertexID;
};

struct VertexOutput {
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    float2 camPos : TEXCOORD1;
};

// Extents of grid in world coordinates
static const float gridSize = 100.0;

// Size of one cell
static const float gridCellSize = 0.025;

// Color of thin lines
static const float4 gridColorThin = float4(0.5, 0.5, 1.0, 1.0);

// Color of thick lines (every tenth line)
static const float4 gridColorThick = float4(0.0, 0.0, 0.0, 1.0);

// Minimum number of pixels between cell lines before LOD switch
static const float gridMinPixelsBetweenCells = 1.0;

// Grid positions
static const float3 pos[4] = {
    float3(-1.0, 0.0, -1.0),
    float3( 1.0, 0.0, -1.0),
    float3( 1.0, 0.0,  1.0),
    float3(-1.0, 0.0,  1.0)
};

// Grid indices
static const int indices[6] = { 
    0, 1, 2, 
    2, 3, 0 
};


// Logarithm base 10
float log10(float x) {
    return log(x) / log(10.0);
}

// Saturate scalar value (clamp to [0, 1])
float satf(float x) {
    return saturate(x);
}

// Saturate vector value (clamp each component to [0, 1])
float2 satv(float2 x) {
    return clamp(x, float2(0.0, 0.0), float2(1.0, 1.0));
}

// Maximum of two components in a vector
float max2(float2 v) {
    return max(v.x, v.y);
}

// Grid color calculation based on UV and camera position
float4 gridColor(float2 uv, float2 camPos) {
    // Screen-space derivatives of UV
    float2 dudv = float2(
        length(float2(ddx(uv.x), ddy(uv.x))),
        length(float2(ddx(uv.y), ddy(uv.y)))
    );

    // Calculate the LOD level
    float lodLevel = max(0.0, log10((length(dudv) * gridMinPixelsBetweenCells) / gridCellSize) + 1.0);
    float lodFade = frac(lodLevel);

    // Cell sizes for LODs
    float lod0 = gridCellSize * pow(10.0, floor(lodLevel));
    float lod1 = lod0 * 10.0;
    float lod2 = lod1 * 10.0;

    // Increase line width for anti-aliasing
    dudv *= 4.0;

    // Offset UV to center anti-aliased lines
    uv += dudv / 2.0f;

    // Calculate alpha coverage for each LOD
    float lod0a = max2(float2(1.0, 1.0) - abs(satv(fmod(uv, lod0) / dudv) * 2.0 - float2(1.0, 1.0)));
	float lod1a = max2(float2(1.0, 1.0) - abs(satv(fmod(uv, lod1) / dudv) * 2.0 - float2(1.0, 1.0)));
    float lod2a = max2(float2(1.0, 1.0) - abs(satv(fmod(uv, lod2) / dudv) * 2.0 - float2(1.0, 1.0)));

    // Offset UV by camera position
    uv -= camPos;

    // Blend between colors for LOD transition
    float4 c = (lod2a > 0.0) 
        ? gridColorThick 
        : (lod1a > 0.0) 
            ? lerp(gridColorThick, gridColorThin, lodFade) 
            : gridColorThin;

    // Calculate opacity falloff based on distance to grid extents
    float opacityFalloff = (1.0 - satf(length(uv) / gridSize));

    // Final alpha blending
    c.a *= (lod2a > 0.0 ? lod2a : lod1a > 0.0 ? lod1a : (lod0a * (1.0 - lodFade))) * opacityFalloff;

    return c;
}

///////////////////////MAIN VS////////////////////////
VertexOutput main_vs(VertexInput input) {
    VertexOutput output;

    // Compute Model-View-Projection matrix
    float4x4 MVP = mul(view, proj);

    // Get vertex index
    int idx = indices[input.vertexID];
    float3 position = pos[idx] * gridSize;

    // Offset by camera position
    position.x += cameraPos.x;
    position.z += cameraPos.z;

    // Set outputs
    output.position = mul( float4(position, 1.0),MVP);
    output.uv = position.xz;
    output.camPos = cameraPos.xz;

    return output;
}

///////////////////////////////////////////
////////////////MAIN PS////////////////////
///////////////////////////////////////////
struct PSInput {
    float2 uv : TEXCOORD0;    // Entrada de las coordenadas UV
    float2 camPos : TEXCOORD1; // Entrada de la posición de la cámara
};

float4 main_ps(PSInput input) : SV_Target {
    return gridColos(input.uv, input.camPos); //  `gridColor`
}



)";




		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.EntryPoint = L"main_vs";
			DescriptionShader.Profile = L"vs_6_0";
			DescriptionShader.Type = ShaderType::VERTEX_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = infinite_grid_shader;
			DescriptionShader.FileName = "infinite_grid_vs";
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.EntryPoint = L"main_ps";
			DescriptionShader.Profile = L"ps_6_0";
			DescriptionShader.Type = ShaderType::PIXEL_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = infinite_grid_shader;
			DescriptionShader.FileName = "infinite_grid_ps";
			m_ps->SetShaderDescription(DescriptionShader);
		}

	/*ShaderCreateDescription DescriptionShader;
	{
		DescriptionShader.NoCompile = true;
		DescriptionShader.FileName = "GL01_grid.vert";

		m_vs->SetShaderDescription(DescriptionShader);
	}

	{
		DescriptionShader.NoCompile = true;
		DescriptionShader.FileName = "GL01_grid.frag";
		m_ps->SetShaderDescription(DescriptionShader);
	}*/




		ShaderCompiler::CompileShader(m_vs.get());
		ShaderCompiler::CompileShader(m_ps.get());


		GraphicsPipelineDescription gdesc;
		gdesc.SetVertexShader(m_vs.get()).
			SetPixelShader(m_ps.get());

		m_GraphicsPipeline = m_ManagerDevice->GetRenderDevice()->CreateGraphicsPipeline(gdesc);

		BufferDescription DescriptionBuffer;
		DescriptionBuffer.SetBufferType(BufferType::CONSTANT_BUFFER)
			.SetByteSize(sizeof(PerFrameData))
			.SetDebugName("UBO MVP PERFRAME DATA ")
			.SetStrideSize(0);

		m_UBO = m_ManagerDevice->GetRenderDevice()->CreateBuffer(DescriptionBuffer);

	}
	
	void Render() override
	{
		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());
		IFNITY_LOG(LogApp, INFO, "Mesh loaded, ready to render");
		


		const mat4 p = glm::perspective(45.0f, aspectRatio, 0.5f, 1000.0f);
		const mat4 view = m_camera.getViewMatrix();

		const PerFrameData perFrameData = { .view = view, .proj = p, .cameraPos = glm::vec4(m_camera.getPosition(), 1.0f) };

		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, &perFrameData, sizeof(PerFrameData));

		//Draw Description 
		DrawDescription desc;
		ViewPortState descViewPort;
		descViewPort.width = m_ManagerDevice->GetWidth();
		descViewPort.height = m_ManagerDevice->GetHeight();
		descViewPort.minDepth = 0.0f;
		descViewPort.maxDepth = 1.0f;

		desc.SetViewPortState(descViewPort);
		//desc.rasterizationState.fillMode = FillModeType::Wireframe;
		desc.size = 6;  //Two triangles 
		desc.indices = (const void*)(0);
		desc.isIndexed = false;

		m_ManagerDevice->GetRenderDevice()->Draw(desc);
		
		
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}
	~Source() override {}


private:
	BufferHandle m_UBO;
	BufferHandle m_VertexBuffer;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_GraphicsPipeline;
	MeshObjectHandle m_MeshTetrahedron;
	MeshObjectHandle m_MeshCube;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;

	// Camera objects 
	IFNITY::EventCameraListener m_CameraListener;
	CameraPositioner_FirstPerson m_camera;


};

class Source_TestD3D12 : public IFNITY::App
{
public:
	Source_TestD3D12(IFNITY::rhi::GraphicsAPI api) : IFNITY::App(api)
	{
		PushLayer(new ExampleLayer());
	}
	~Source_TestD3D12() override
	{
	}
};


IFNITY::App* IFNITY::CreateApp()
{


	auto api = IFNITY::rhi::GraphicsAPI::OPENGL;


	
	return new Source(api);
}

