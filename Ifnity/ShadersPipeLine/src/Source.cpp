

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


class Source_Cube: public IFNITY::App
{

public:

	Source_Cube(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr())
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

static const float3 g_positions[8] = {
    float3(-1.0, -1.0, 1.0),
    float3(1.0, -1.0, 1.0),
    float3(1.0, 1.0, 1.0),
    float3(-1.0, 1.0, 1.0),
    float3(-1.0, -1.0, -1.0),
    float3(1.0, -1.0, -1.0),
    float3(1.0, 1.0, -1.0),
    float3(-1.0, 1.0, -1.0)
};

static const float3 g_colors[8] = {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0),
    float3(1.0, 1.0, 0.0),
    float3(1.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0),
    float3(0.0, 1.0, 0.0),
    float3(1.0, 0.0, 0.0)
};

static const uint indices[36] = {
    // front
    0, 1, 2, 2, 3, 0,
    // right
    1, 5, 6, 6, 2, 1,
    // back
    7, 6, 5, 5, 4, 7,
    // left
    4, 0, 3, 3, 7, 4,
    // bottom
    4, 5, 1, 1, 0, 4,
    // top
    3, 2, 6, 6, 7, 3
};
	void main_vs(
		  uint i_vertexId : SV_VertexID,
    out float4 o_pos : SV_Position,
    out float3 o_color : COLOR
	)
	{
		uint index = indices[i_vertexId];
	    o_pos = mul(float4(g_positions[index], 1.0), MVP);
	    o_color = g_colors[index];
	}

	void main_ps(
    in float4 i_pos : SV_Position,
    in float3 i_color : COLOR,
    out float4 o_color : SV_Target0
)
{
    o_color = float4(i_color, 1.0);
}
	)"
			;



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

		const mat4 mg = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -3.5f)), App::GetTime(), vec3(1.0f, 1.0f, 1.0f));
		const mat4 fg = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);
		alignas(16) const mat4 mvpg = fg * mg;



		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, glm::value_ptr(mvpg), sizeof(mvpg));
		//m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, mvpd.m_data, sizeof(mvpd));

		//Draw Description 
		DrawDescription desc;
		desc.size = 36;
		m_ManagerDevice->GetRenderDevice()->Draw(desc);

		IFNITY_LOG(LogApp, INFO, "Render App");
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}
	~Source_Cube() override {}

private:
	BufferHandle m_UBO;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_GraphicsPipeline;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;
};

//This is global 


class Source_Tetahedre: public IFNITY::App
{

public:

	Source_Tetahedre(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr())
	{

		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 


	}

	void Initialize() override
	{
		m_ManagerDevice = &App::GetApp().GetManagerDevice();


		IFNITY::ShaderCompiler::Initialize();



		/*	std::wstring shaderSource3 = LR"(

		cbuffer PerFrameData : register(b0)
	{
		matrix MVP;
	};

	static const float2 g_positions[] =
	{
		float2(-0.5, -0.5),
		float2(0, 0.5),
		float2(0.5, -0.5),

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
	)";*/
	/*std::wstring shaderSource3 = LR"(

cbuffer PerFrameData : register(b0)
{
	matrix MVP;
};

struct VertexInput
{
	float3 position : POSITION0;
	float3 color : COLOR1;
};

static const float3 g_colors[] =
{
	float3(1, 1, 1),
	float3(0, 1, 0),
	float3(0, 0, 1)

};

void main_vs(
	in VertexInput input,
	uint i_vertexId : SV_VertexID,
	out float4 o_pos : SV_Position,
	out float3 o_color : COLOR
)
{
	  o_pos = mul(float4(input.position, 1.0),MVP);
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
)";*/

		std::wstring shaderSource3 = LR"(

		cbuffer PerFrameData : register(b0)
		{
			matrix MVP;
		};
		
		struct VertexInput
		{
		    float3 position : POSITION0;
		    float3 color : COLOR1; // Color de entrada desde el vértice
		};
		
		// Función del Vertex Shader
		void main_vs(
		    VertexInput input,                 // Entrada del vértice
		    uint i_vertexId : SV_VertexID,     // ID del vértice
		    out float4 o_pos : SV_Position,     // Salida de posición
		    out float3 o_color : COLOR          // Salida de color
		)
		{
		    // Multiplicar la posición por la matriz MVP
		    o_pos = mul(float4(input.position, 1.0), MVP);
		    // Asignar el color desde la entrada del vértice
		    o_color = input.color; // Usa el color directamente de la entrada
		}
		
		// Función del Pixel Shader
		void main_ps(
		    float4 i_pos : SV_Position,          // Entrada de posición (no se usa en este caso)
		    float3 i_color : COLOR,              // Entrada de color
		    out float4 o_color : SV_Target0       // Salida de color
		)
		{
		    // Asignar el color a la salida del pixel
		    o_color = float4(i_color, 1.0); 
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



		size_t sizeIndices = m_Tetrahedron.GetSizeIndices();
		size_t sizeVertices = m_Tetrahedron.GetSizeVertices();

		m_VertexBuffer = m_ManagerDevice->GetRenderDevice()->CreateBuffer(
			BufferDescription()
			.SetBufferType(BufferType::VERTEX_INDEX_BUFFER)
			.SetByteSize(sizeVertices + sizeIndices)
			.SetDebugName("Tetrahedron")
		);



		VertexAttributeDescription indexDescription;
		indexDescription.setName("Index")
			.setOffset(sizeIndices)
			.setElementStride(sizeof(GeometricModels::Vertex))
			.setHaveIndexBuffer(true);
		//.setData(m_Tetrahedron.GetIndices().data())


		VertexAttributeDescription vertexDescription;
		vertexDescription.setName("Vertex Position")
			.setOffset(offsetof(GeometricModels::Vertex, position))
			.setArraySize(3)
			.setIsInstanced(false)
			.setBufferIndexLocation(0)
			.setSize(sizeof(vec3));

		VertexAttributeDescription normalDescription;
		normalDescription.setName("Vertex Normal")
			.setOffset(offsetof(GeometricModels::Vertex, normal))
			.setArraySize(3)
			.setIsInstanced(false)
			.setBufferIndexLocation(1)
			.setSize(sizeof(vec3));

		VertexAttributeDescription tangentDescription;
		tangentDescription.setName("Vertex Tangent")
			.setOffset(offsetof(GeometricModels::Vertex, tangent))
			.setArraySize(3)
			.setIsInstanced(false)
			.setBufferIndexLocation(2)
			.setSize(sizeof(vec3));


		VertexAttributeDescription vertxAtt[ 4 ] = {
			indexDescription,
			vertexDescription,
			normalDescription,
			tangentDescription
		};

		m_ManagerDevice->GetRenderDevice()->BindingVertexIndexAttributes(vertxAtt, 4, m_VertexBuffer);

		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_VertexBuffer, m_Tetrahedron.index.data(), sizeIndices, 0);
		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_VertexBuffer, m_Tetrahedron.vertices.data(), sizeVertices, sizeIndices);




	}

	void Render() override
	{
		using namespace math;
		//SetPipelineState
		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());

		const mat4 mg = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -3.5f)), (float)glfwGetTime(), vec3(1.0f, 1.0f, 1.0f));
		const mat4 fg = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);
		const mat4 mvpg = fg * mg;





		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, glm::value_ptr(mvpg), sizeof(mvpg));
		//m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, mvpd.m_data, sizeof(mvpd));

		//Draw Description 
		DrawDescription desc;
		//desc.rasterizationState.fillMode = FillModeType::Wireframe;
		desc.size = m_Tetrahedron.index.size();
		desc.indices = (const void*)(0);
		desc.isIndexed = true;

		m_ManagerDevice->GetRenderDevice()->Draw(desc);

		IFNITY_LOG(LogApp, INFO, "Render App");
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}
	~Source_Tetahedre() override {}

private:
	BufferHandle m_UBO;
	BufferHandle m_VertexBuffer;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_GraphicsPipeline;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;

	GeometricModels::Tetrahedron m_Tetrahedron;
};


//-------------------------------------------------//
//  SOURCE  attib normal                             //
//-------------------------------------------------//

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
		
		struct VertexInput
		{
		    float3 position : POSITION0;
		    float3 color : COLOR1; // Color de entrada desde el vértice
		};
		
		// Función del Vertex Shader
		void main_vs(
		    VertexInput input,                 // Entrada del vértice
		    uint i_vertexId : SV_VertexID,     // ID del vértice
		    out float4 o_pos : SV_Position,     // Salida de posición
		    out float3 o_color : COLOR          // Salida de color
		)
		{
		    // Multiplicar la posición por la matriz MVP
		    o_pos = mul(float4(input.position, 1.0), MVP);
		    // Asignar el color desde la entrada del vértice
		    o_color = input.color; // Usa el color directamente de la entrada
		}
		
		// Función del Pixel Shader
		void main_ps(
		    float4 i_pos : SV_Position,          // Entrada de posición (no se usa en este caso)
		    float3 i_color : COLOR,              // Entrada de color
		    out float4 o_color : SV_Target0       // Salida de color
		)
		{
		    // Asignar el color a la salida del pixel
		    o_color = float4(i_color, 1.0); 
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



		// Definir las posiciones de los vértices
		std::vector<vec3> vertexPositions = {
			vec3(-0.5f, -0.5f, 0.0f),
			vec3(1.0f, 0.5f, 0.0f),
			vec3(0.5f, -0.5f, 0.0f)
		};
		std::vector<vec3> vertexColors = {
			   vec3(1.0f, 0.0f, 0.0f),
			   vec3(0.0f, 1.0f, 0.0f),
			   vec3(0.0f, 0.0f, 1.0f)
		};

		struct Vertex
		{
			vec3 position;
			vec3 color;
		};

		std::vector<Vertex> vertices = {
		   {{0.0f,  0.5f, 0.0f},  {1.0f, 0.0f, 0.0f}},  // Vértice superior (rojo)
		   {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Vértice inferior izquierdo (verde)
		   {{0.5f, -0.5f, 0.0f},  {.0f, .0f, 1.0f}}   // Vértice inferior derecho (azul)
		};

		VertexAttributeDescription vertexDescription;
		vertexDescription.setName("Vertex Position")
			.setOffset(0)
			.setArraySize(3)
			.setIsInstanced(false)
			.setBufferIndexLocation(0)
			.setElementStride(sizeof(Vertex))
			.setSize(sizeof(vec3))
			.setData(vertices.data());

		VertexAttributeDescription colorDescription;
		colorDescription.setName("Vertex Color")
			.setOffset(sizeof(vec3))           // Offset del color en la estructura Vertex
			.setArraySize(3)                        // Número de componentes de color (r, g, b)
			.setIsInstanced(false)
			.setBufferIndexLocation(1)                      // bufferIndex 1
			.setElementStride(sizeof(Vertex))       // *Stride* de cada vértice
			.setSize(sizeof(vec3));

		VertexAttributeDescription vertxAtt[ 2 ] = { vertexDescription,colorDescription };

		m_ManagerDevice->GetRenderDevice()->BindingVertexAttributes(vertxAtt, 2, vertexDescription.data, vertices.size() * sizeof(Vertex));




	}





	void Render() override
	{
		using namespace math;
		//SetPipelineState
		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());

		const mat4 mg = glm::rotate(mat4(1.0f), (float)glfwGetTime(), vec3(0.0f, 0.0f, -1.0f));
		const mat4 fg = glm::ortho(-aspectRatio, aspectRatio, -1.f, 1.f, 1.f, -1.f);
		const mat4 mvpg = fg * mg;



		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, glm::value_ptr(mvpg), sizeof(mvpg));

		//Draw Description 
		DrawDescription desc;
		//desc.rasterizationState.fillMode = FillModeType::Wireframe;
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
	GraphicsPipelineHandle m_GraphicsPipeline;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;
};


//-------------------------------------------------//
//  SOURCE_TEXTURE                                  //
//-------------------------------------------------//

class Source_Texture: public IFNITY::App
{

public:

	Source_Texture(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr())
	{

		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 


	}

	void Initialize() override
	{
		m_ManagerDevice = &App::GetApp().GetManagerDevice();


		IFNITY::ShaderCompiler::Initialize();

		//
		std::wstring shaderSource3 = LR"(

			// Buffer constante para la matriz MVP
cbuffer PerFrameData : register(b0)
{
    matrix MVP;
};

// Coordenadas de vértices
static const float2 g_positions[3] =
{
    float2(-0.5, -0.5),
    float2(0.0, 0.5),
    float2(0.5, -0.5)
};

// Coordenadas de textura (UV)
static const float2 tc[3] = 
{
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.5f, 1.0f)
};

// Estructura de salida del Vertex Shader
struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

// Vertex Shader
VSOutput main_vs(uint i_vertexId : SV_VertexID)
{
    VSOutput output;
    output.pos = mul(float4(g_positions[i_vertexId], 0.0f, 1.0f), MVP);
    output.uv = tc[i_vertexId];
    return output;
}

// Definición de la textura y el sampler en HLSL
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

// Pixel Shader
float4 main_ps(VSOutput input) : SV_Target
{
    // Muestra el color de la textura usando las coordenadas UV
    return texture0.Sample(sampler0, input.uv);
}

	)";





		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);
		vfs.Mount("Data", "Data", IFNITY::FolderType::TEXTURES);

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




		m_Texture = m_ManagerDevice->GetRenderDevice()->CreateTexture
		(TextureDescription().setFilePath("Data/diffuse_madera.jpg"));




	}





	void Render() override
	{
		using namespace math;
		//SetPipelineState
		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());



		const mat4 mg = glm::rotate(mat4(1.0f), (float)glfwGetTime(), vec3(0.0f, 0.0f, -1.0f));
		const mat4 fg = glm::ortho(-aspectRatio, aspectRatio, -1.f, 1.f, 1.f, -1.f);
		const mat4 mvpg = fg * mg;



		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, glm::value_ptr(mvpg), sizeof(mvpg));

		//Draw Description 
		DrawDescription desc;
		//desc.rasterizationState.fillMode = FillModeType::Wireframe;
		desc.size = 3;
		m_ManagerDevice->GetRenderDevice()->Draw(desc);

		IFNITY_LOG(LogApp, INFO, "Render App");
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}
	~Source_Texture() override {}

private:
	TextureHandle m_Texture;
	BufferHandle m_UBO;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_GraphicsPipeline;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;
};


//-------------------------------------------------//
//  SOURCE_VERTEXPULLING                           //
//-------------------------------------------------//

class Source_VTXP: public IFNITY::App
{

public:

	Source_VTXP(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr())
	{

		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 


	}

	void Initialize() override
	{
		m_ManagerDevice = &App::GetApp().GetManagerDevice();


		IFNITY::ShaderCompiler::Initialize();



		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);
		vfs.Mount("Data", "Data", IFNITY::FolderType::TEXTURES);

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();
		m_gs = std::make_shared<IShader>();

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "vtxpulling";
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{

			DescriptionShader.FileName = "ptxpulling";
			DescriptionShader.NoCompile = true;
			m_ps->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.FileName = "gtxpulling";
			DescriptionShader.NoCompile = true;
			m_gs->SetShaderDescription(DescriptionShader);

		}

		ShaderCompiler::CompileShader(m_vs.get());
		ShaderCompiler::CompileShader(m_ps.get());
		ShaderCompiler::CompileShader(m_gs.get());


		GraphicsPipelineDescription gdesc;
		gdesc.SetVertexShader(m_vs.get())
			.SetPixelShader(m_ps.get())
			.SetGeometryShader(m_gs.get());

		m_GraphicsPipeline = m_ManagerDevice->GetRenderDevice()->CreateGraphicsPipeline(gdesc);

		BufferDescription DescriptionBuffer;
		DescriptionBuffer.SetBufferType(BufferType::CONSTANT_BUFFER)
			.SetByteSize(sizeof(mat4))
			.SetDebugName("UBO MVP")
			.SetStrideSize(0);
		m_UBO = m_ManagerDevice->GetRenderDevice()->CreateBuffer(DescriptionBuffer);

		//Assimp process
		struct VertexData
		{
			vec3 pos;
			vec2 tc;
		};
		const aiScene* scene = aiImportFile("data/rubber_duck/scene.gltf", aiProcess_Triangulate);


		if(!scene || !scene->HasMeshes())
		{
			printf("Unable to load data/rubber_duck/scene.gltf\n");
			exit(255);
		}



		const aiMesh* mesh = scene->mMeshes[ 0 ];
		std::vector<VertexData> vertices;
		for(unsigned i = 0; i != mesh->mNumVertices; i++)
		{
			const aiVector3D v = mesh->mVertices[ i ];
			const aiVector3D t = mesh->mTextureCoords[ 0 ][ i ];

			vertices.push_back(VertexData{ vec3(v.x, v.z, v.y), vec2(t.x, t.y) });
		}
		std::vector<unsigned int> indices;
		for(unsigned i = 0; i != mesh->mNumFaces; i++)
		{
			for(unsigned j = 0; j != 3; j++)
				indices.push_back(mesh->mFaces[ i ].mIndices[ j ]);
		}
		aiReleaseImport(scene);

		const size_t kSizeIndices = sizeof(unsigned int) * indices.size();
		const size_t kSizeVertices = sizeof(VertexData) * vertices.size();
		m_IndexCount = indices.size();

		m_IndexBuffer = m_ManagerDevice->GetRenderDevice()->CreateBuffer(
			BufferDescription()
			.SetBufferType(BufferType::VERTEX_PULLING_BUFFER_INDEX)
			.SetByteSize(kSizeIndices)
			.SetDebugName("Assimp_Index")
		);

		m_VertexBuffer = m_ManagerDevice->GetRenderDevice()->CreateBuffer(
			BufferDescription()
			.SetBindingPoint(1)
			.SetByteSize(kSizeVertices)
			.SetDebugName("Assimp_Vertex")
			.SetBufferType(BufferType::VERTEX_PULLING_BUFFER));


		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_IndexBuffer, indices.data(), kSizeIndices, 0);
		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_VertexBuffer, vertices.data(), kSizeVertices, 0);

		m_Texture = m_ManagerDevice->GetRenderDevice()->CreateTexture
		(TextureDescription().setFilePath("data/rubber_duck/textures/Duck_baseColor.png"));

	}





	void Render() override
	{
		//using namespace math;
		////SetPipelineState
		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());



		const mat4 m = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, -0.5f, -1.5f)), (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
		const mat4 p = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);

		const mat4 mvpg = p * m;


		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, glm::value_ptr(mvpg), sizeof(mvpg));

		//Draw Description 
		DrawDescription desc;
		//desc.rasterizationState.fillMode = FillModeType::Wireframe;
		desc.size = m_IndexCount;
		desc.indices = (const void*)(0);
		desc.isIndexed = true;

		m_ManagerDevice->GetRenderDevice()->Draw(desc);

		////Draw Description 
		//DrawDescription desc;
		////desc.rasterizationState.fillMode = FillModeType::Wireframe;
		//desc.size = 3;
		//m_ManagerDevice->GetRenderDevice()->Draw(desc);

		IFNITY_LOG(LogApp, INFO, "Render App");
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}

	void LoadAssimp()
	{

	}
	~Source_VTXP() override {}

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
};

//-------------------------------------------------//
//  SOURCE_VERTEXPULLING_HLSL                           //
//-------------------------------------------------//

class Source_VTXP_HLSL: public IFNITY::App
{
	ImGuiTestLayer* m_ImGuiLayer;

public:

	struct PerFrameData
	{
		mat4 MVP;
		float thickness;

	};

	Source_VTXP_HLSL(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr())
	{

		PushLayer(new   IFNITY::NVML_Monitor());
		m_ImGuiLayer = new ImGuiTestLayer();
		PushLayer(m_ImGuiLayer);
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 


	}

	void Initialize() override
	{
		m_ManagerDevice = &App::GetApp().GetManagerDevice();


		IFNITY::ShaderCompiler::Initialize();



		std::wstring shaderSource3 = LR"(

cbuffer PerFrameData : register(b0)
{
    float4x4 MVP;
	float thickness;
};

struct Vertex
{
     float p[3];  // Position as a float3
     float tc[2]; // Texture coordinates as a float2
};

// Structured buffer for vertices, bound to register t1
StructuredBuffer<Vertex> in_Vertices : register(t1);

float3 getPosition(uint i)
{
    // Return position as a float3 using the array values
    return float3(in_Vertices[i].p[0], in_Vertices[i].p[1], in_Vertices[i].p[2]);
}

float2 getTexCoord(uint i)
{
    // Return texture coordinates as a float2 using the array values
    return float2(in_Vertices[i].tc[0], in_Vertices[i].tc[1]);
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT main_vs(uint VertexID : SV_VertexID)
{
    VS_OUTPUT output;

    // Fetch the position and texture coordinates
    float3 pos = getPosition(VertexID);
    output.pos = mul( float4(pos, 1.0),MVP);
    
    output.uv = getTexCoord(VertexID);

    return output;
}
/////////////////////////////////////////
//SHADER GEOM ///////////////////////////
/////////////////////////////////////////

struct GSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct GSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 barycoords : TEXCOORD1;
};

[maxvertexcount(3)]
void main_gs(triangle GSInput input[3], inout TriangleStream<GSOutput> triStream)
{
    const float3 bc[3] = {
        float3(1.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        float3(0.0, 0.0, 1.0)
    };

    for (int i = 0; i < 3; i++)
    {
        GSOutput output;
        output.position = input[i].position;
        output.uv = input[i].uv;
        output.barycoords = bc[i];
        triStream.Append(output);
    }
    triStream.RestartStrip();
}

/////////////////////////////////////////
//PIXEL SHADER ///////////////////////////
/////////////////////////////////////////



Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);



struct PSInput
{
    float2 uvs : TEXCOORD0;
    float3 barycoords : TEXCOORD1;
};

float edgeFactor(float thickness, float3 barycoords)
{
    float3 a3 = smoothstep(float3(0.0, 0.0, 0.0), fwidth(barycoords) * thickness, barycoords);
    return min(min(a3.x, a3.y), a3.z);
}

float4 main_ps(PSInput input) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, input.uvs);
    return lerp(color * float4(0.8, 0.8, 0.8, 1.0), color, edgeFactor(thickness, input.barycoords));
}
	)";

		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);
		vfs.Mount("Data", "Data", IFNITY::FolderType::TEXTURES);

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();
		m_gs = std::make_shared<IShader>();

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.EntryPoint = L"main_vs";
			DescriptionShader.Profile = L"vs_6_0";
			DescriptionShader.Type = ShaderType::VERTEX_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = shaderSource3;
			DescriptionShader.FileName = "vtxpulling";
			DescriptionShader.NoCompile = false;
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{

			DescriptionShader.EntryPoint = L"main_ps";
			DescriptionShader.Profile = L"ps_6_0";
			DescriptionShader.Type = ShaderType::PIXEL_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = shaderSource3;
			DescriptionShader.FileName = "ptxpulling";
			DescriptionShader.NoCompile = false;
			m_ps->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.EntryPoint = L"main_gs";
			DescriptionShader.Profile = L"gs_6_0";
			DescriptionShader.Type = ShaderType::GEOMETRY_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = shaderSource3;
			DescriptionShader.FileName = "gtxpulling";
			DescriptionShader.NoCompile = false;
			m_gs->SetShaderDescription(DescriptionShader);

		}

		ShaderCompiler::CompileShader(m_vs.get());
		ShaderCompiler::CompileShader(m_ps.get());
		ShaderCompiler::CompileShader(m_gs.get());


		GraphicsPipelineDescription gdesc;
		gdesc.SetVertexShader(m_vs.get())
			.SetPixelShader(m_ps.get())
			.SetGeometryShader(m_gs.get());

		m_GraphicsPipeline = m_ManagerDevice->GetRenderDevice()->CreateGraphicsPipeline(gdesc);

		BufferDescription DescriptionBuffer;
		DescriptionBuffer.SetBufferType(BufferType::CONSTANT_BUFFER)
			.SetByteSize(sizeof(PerFrameData))
			.SetDebugName("UBO MVP")
			.SetStrideSize(0);
		m_UBO = m_ManagerDevice->GetRenderDevice()->CreateBuffer(DescriptionBuffer);

		//Assimp process
		struct VertexData
		{
			vec3 pos;
			vec2 tc;
		};
		const aiScene* scene = aiImportFile("data/rubber_duck/scene.gltf", aiProcess_Triangulate);


		if(!scene || !scene->HasMeshes())
		{
			printf("Unable to load data/rubber_duck/scene.gltf\n");
			exit(255);
		}



		const aiMesh* mesh = scene->mMeshes[ 0 ];
		std::vector<VertexData> vertices;
		for(unsigned i = 0; i != mesh->mNumVertices; i++)
		{
			const aiVector3D v = mesh->mVertices[ i ];
			const aiVector3D t = mesh->mTextureCoords[ 0 ][ i ];

			vertices.push_back(VertexData{ vec3(v.x, v.z, v.y), vec2(t.x, t.y) });
		}
		std::vector<unsigned int> indices;
		for(unsigned i = 0; i != mesh->mNumFaces; i++)
		{
			for(unsigned j = 0; j != 3; j++)
				indices.push_back(mesh->mFaces[ i ].mIndices[ j ]);
		}
		aiReleaseImport(scene);

		const size_t kSizeIndices = sizeof(unsigned int) * indices.size();
		const size_t kSizeVertices = sizeof(VertexData) * vertices.size();
		m_IndexCount = indices.size();

		m_IndexBuffer = m_ManagerDevice->GetRenderDevice()->CreateBuffer(
			BufferDescription()
			.SetBufferType(BufferType::VERTEX_PULLING_BUFFER_INDEX)
			.SetByteSize(kSizeIndices)
			.SetDebugName("Assimp_Index")
		);

		m_VertexBuffer = m_ManagerDevice->GetRenderDevice()->CreateBuffer(
			BufferDescription()
			.SetBindingPoint(1)
			.SetByteSize(kSizeVertices)
			.SetDebugName("Assimp_Vertex")
			.SetBufferType(BufferType::VERTEX_PULLING_BUFFER));


		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_IndexBuffer, indices.data(), kSizeIndices, 0);
		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_VertexBuffer, vertices.data(), kSizeVertices, 0);

		m_Texture = m_ManagerDevice->GetRenderDevice()->CreateTexture
		(TextureDescription().setFilePath("data/rubber_duck/textures/Duck_baseColor.png"));

	}





	void Render() override
	{
		//using namespace math;
		////SetPipelineState
		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());



		const mat4 m = glm::rotate(glm::translate(mat4(1.0f), vec3(0.0f, -0.5f, -1.5f)), (float)glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
		const mat4 p = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);

		const mat4 mvpg = p * m;

		PerFrameData data;
		data.MVP = mvpg;
		data.thickness = m_ImGuiLayer->GetThickness();

		const void* dataPtr = static_cast<const void*>(&data);




		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, dataPtr, sizeof(PerFrameData));

		//Draw Description 
		DrawDescription desc;
		//desc.rasterizationState.fillMode = FillModeType::Wireframe;
		desc.size = m_IndexCount;
		desc.indices = (const void*)(0);
		desc.isIndexed = true;

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
	~Source_VTXP_HLSL() override {}

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
};

//------------------------------------------------- //
//  SOURCE_CUBEMAP                                  //
//------------------------------------------------- //

class Source_CUBEMAP: public IFNITY::App
{

public:

	struct PerFrameData
	{
		mat4 model;
		mat4 mvp;
		vec4 cameraPos;
	};

	Source_CUBEMAP(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr())
	{

		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
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
		//using namespace math;
		////SetPipelineState
		float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());


		const mat4 m = glm::scale(mat4(1.0f), vec3(2.0f));
		const mat4 p = glm::perspective(45.0f, aspectRatio, 0.1f, 1000.0f);

		const PerFrameData perFrameData
		{

			m, //model
			p * m, //mvp
			vec4(0.0f, 0.0f, 0.0f, 0.0f) //cameraPos
		};


		


		m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, &perFrameData, sizeof(PerFrameData));

		//Draw Description 
		DrawDescription desc;
		//desc.rasterizationState.fillMode = FillModeType::Wireframe;
		desc.size = 36; // Cube vertex 
		desc.indices = (const void*)(0);
		desc.isIndexed = false;
		desc.viewPortState = ViewPortState(0, 0, m_ManagerDevice->GetWidth(), m_ManagerDevice->GetHeight());

		m_ManagerDevice->GetRenderDevice()->Draw(desc);

		////Draw Description 
		//DrawDescription desc;
		////desc.rasterizationState.fillMode = FillModeType::Wireframe;
		//desc.size = 3;
		//m_ManagerDevice->GetRenderDevice()->Draw(desc);

		IFNITY_LOG(LogApp, INFO, "Render App");
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}

	void LoadAssimp()
	{

	}
	~Source_CUBEMAP() override {}

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
};

void error_callback(void*, const char* error_message)
{
	// Handle the error message here  
	std::cerr << "Error: " << error_message << std::endl;
}

IFNITY::App* IFNITY::CreateApp()
{


	auto api = IFNITY::rhi::GraphicsAPI::OPENGL;

	//return new Source_Texture(api)
	//return new Source_Tetahedre(api);
	//
	// return new Source_Tetahedre(api);

	//return new Source_VTXP_HLSL(api);
	return new Source_CUBEMAP(api);
}

