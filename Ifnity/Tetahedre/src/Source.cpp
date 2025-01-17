

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
	// Heredado v�a Layer
	void ConnectToEventBusImpl(void* bus) override
	{}

	// M�todo para obtener el valor de thickness
	float GetThickness() const { return m_Thickness; }
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

				// Actualizar el estado de rasterizaci�n
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
		    float3 color : COLOR1; // Color de entrada desde el v�rtice
		};
		
		// Funci�n del Vertex Shader
		void main_vs(
		    VertexInput input,                 // Entrada del v�rtice
		    uint i_vertexId : SV_VertexID,     // ID del v�rtice
		    out float4 o_pos : SV_Position,     // Salida de posici�n
		    out float3 o_color : COLOR          // Salida de color
		)
		{
		    // Multiplicar la posici�n por la matriz MVP
		    o_pos = mul(float4(input.position, 1.0), MVP);
		    // Asignar el color desde la entrada del v�rtice
		    o_color = input.color; // Usa el color directamente de la entrada
		}
		
		// Funci�n del Pixel Shader
		void main_ps(
		    float4 i_pos : SV_Position,          // Entrada de posici�n (no se usa en este caso)
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
			.setElementStride(sizeof(GeometricModels::VertexGeometry))
			.setHaveIndexBuffer(true);
		//.setData(m_Tetrahedron.GetIndices().data())


		VertexAttributeDescription vertexDescription;
		vertexDescription.setName("Vertex Position")
			.setOffset(0)
			.setArraySize(3)
			.setIsInstanced(false)
			.setBufferIndexLocation(0)
			.setSize(sizeof(vec3));

		VertexAttributeDescription normalDescription;
		normalDescription.setName("Vertex Normal")
			.setOffset(12)
			.setArraySize(3)
			.setIsInstanced(false)
			.setBufferIndexLocation(1)
			.setSize(sizeof(vec3));

		VertexAttributeDescription tangentDescription;
		tangentDescription.setName("Vertex Tangent")
			.setOffset(24)
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
	 return new Source_Tetahedre(api);

	
}

