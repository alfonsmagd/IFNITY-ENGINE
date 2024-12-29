

// IFNITY.cp


#include <Ifnity.h>
#include <future>
#include "..\..\Ifnity\vendor\glad\include\glad\glad.h"



using namespace IFNITY;
using namespace IFNITY::rhi;
using glm::vec3;
using glm::vec4;
using glm::vec2;
using glm::mat4;

#define SELECT_API_SELECTOR 0





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

		#if SELECT_API_SELECTOR
		ChooseApi();
		#endif
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

};

class Source: public IFNITY::App
{
public:

	struct PerFrameData
	{
		mat4 view;
		mat4 proj;
		vec4 cameraPos;
	};




	Source(IFNITY::rhi::GraphicsAPI api): 
		IFNITY::App(api),
		m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr()),
		m_camera(vec3(-10.5f, 7.5f, -0.f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f)),
		m_CameraListener(&m_camera)
	{

		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(new ImGuiTestLayer());
		PushLayer(new IFNITY::CameraLayer(&m_CameraListener));
		PushOverlay(new IFNITY::ImguiLayer());

		const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);

	}

	void Initialize() override
	{
		IFNITY::ShaderCompiler::Initialize();
		auto& vfs = IFNITY::VFS::GetInstance();




		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);
		vfs.Mount("Data", "Data", IFNITY::FolderType::TEXTURES);



		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();
		m_gs = std::make_shared<IShader>();
		std::wstring shaderSource3 = LR"(

				// HLSL Shader
		cbuffer PerFrameData : register(b0)
		{
		    float4x4 view;
		    float4x4 proj;
		    float4 cameraPos; // No se usa en este shader, pero se declara como en GLSL
		};
		
		StructuredBuffer<float4x4> in_Model : register(t2);
		
		struct VSInput
		{
		    float3 pos : POSITION;
		};
		
		struct VSOutput
		{
		    float4 position : SV_POSITION;
		    float2 uv : TEXCOORD0;
		    float3 wpos : TEXCOORD1;
		};
		
		VSOutput main_vs(VSInput input, uint instanceID : SV_InstanceID)
		{
		    VSOutput output;
		
		    // Calculamos el modelo, vista y proyección
		    float4x4 MVP = mul(in_Model[instanceID], mul(view, proj));
		
		    // Transformamos la posición
		    output.position = mul( float4(input.pos, 1.0),MVP);
		
		    // Coordenadas de textura y posición en el mundo
		    output.uv = float2(0.1, 0.1);
		    output.wpos = input.pos;
		
		    return output;
		}
		

	//GS Shader//////////////////////////////////////////////////////////////////////////////////////
			struct GSInput
			{
			    float4 position : SV_POSITION;
			    float2 uv : TEXCOORD0;
			    float3 wpos : TEXCOORD1;
			};
			
			struct GSOutput
			{
			    float4 position : SV_POSITION;
			    float2 uv : TEXCOORD0;
			    float3 barycoords : TEXCOORD1;
			    float3 wpos_out : TEXCOORD2;
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
			        output.wpos_out = input[i].wpos;
			        triStream.Append(output);
			    }
			    triStream.RestartStrip();
			}

	//PS Shader//////////////////////////////////////////////////////////////////////////////////////
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct PSInput
{
    float2 uvs : TEXCOORD0;
    float3 barycoords : TEXCOORD1;
    float3 wpos_out : TEXCOORD2;
};

float edgeFactor(float thickness, float3 barycoords)
{
    float3 a3 = smoothstep(float3(0.0, 0.0, 0.0), fwidth(barycoords) * thickness, barycoords);
    return min(min(a3.x, a3.y), a3.z);
}

float4 main_ps(PSInput input) : SV_TARGET
{
    float4 color = float4(1.0, 1.0, 1.0, 1.0);
    return lerp(color * float4(0.1, 0.1, 0.1, 1.0), color, edgeFactor(1.0, input.barycoords));
}





)";
			ShaderCreateDescription DescriptionShader;
			{
				DescriptionShader.EntryPoint = L"main_vs";
				DescriptionShader.Profile = L"vs_6_0";
				DescriptionShader.Type = ShaderType::VERTEX_SHADER;
				DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
				DescriptionShader.ShaderSource = shaderSource3;
				DescriptionShader.FileName = "largescene_wireframeVS";
				DescriptionShader.NoCompile = false;
				m_vs->SetShaderDescription(DescriptionShader);
			}
			{

				DescriptionShader.EntryPoint = L"main_ps";
				DescriptionShader.Profile = L"ps_6_0";
				DescriptionShader.Type = ShaderType::PIXEL_SHADER;
				DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
				DescriptionShader.ShaderSource = shaderSource3;
				DescriptionShader.FileName = "largescene_wireframeFS";
				DescriptionShader.NoCompile = false;
				m_ps->SetShaderDescription(DescriptionShader);
			}
			{
				DescriptionShader.EntryPoint = L"main_gs";
				DescriptionShader.Profile = L"gs_6_0";
				DescriptionShader.Type = ShaderType::GEOMETRY_SHADER;
				DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
				DescriptionShader.ShaderSource = shaderSource3;
				DescriptionShader.FileName = "largescene_wireframeGS";
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


			std::string  filenameAssimp = "data/rubber_duck/scene.gltf";
			std::string  meshDataTest = "data/rubber_duck/test.meshes";
			std::string  test = "data/rubber_duck/test.meshes";
			std::string test3 = "data/bistro/Exterior/exterior.obj";
			std::string test3Result = "data/bistro/Exterior/exterior.obj.meshdata";

			std::string cornelbox = "data/Cornel-box/CornellBox-Sphere.obj";

			std::string duck5file = "data/rubber_duck/scene.gltf.meshdata";


			std::string sanmiguel = "data/sanmiguel/san-miguel-low-poly.obj";
			std::string sanmiguelResult = "data/sanmiguel/san-miguel-low-poly.obj.meshdata";



		m_ManagerDevice = &App::GetApp().GetManagerDevice();


		IFNITY::ShaderCompiler::Initialize();



		BufferDescription DescriptionBuffer;
		DescriptionBuffer.SetBufferType(BufferType::CONSTANT_BUFFER)
			.SetByteSize(sizeof(PerFrameData))
			.SetDebugName("UBO MVP PERFRAME DATA ")
			.SetStrideSize(0);

		m_UBO = m_ManagerDevice->GetRenderDevice()->CreateBuffer(DescriptionBuffer);


		
		const mat4 m(glm::scale(mat4(1.0f), vec3(2.0f)));
		//Create PerFrameData Buffer 
		BufferDescription descriptionStorageBuffer;
		descriptionStorageBuffer.SetBufferType(BufferType::STORAGE_BUFFER)
			.SetByteSize(sizeof(mat4))
			.SetData(glm::value_ptr(m))
			.SetDebugName("STORAGE MODEL")
			.SetStrideSize(0)
			.SetBindingPoint(2);

		m_StorageBuffer = m_ManagerDevice->GetRenderDevice()->CreateBuffer(descriptionStorageBuffer);

		//

		
		//meshLoadingFuture = std::async(std::launch::async, &Source::loadMeshAsync, this, filenameAssimp);
		
		MeshObjectDescription meshAssimp =
		{
			.filePath = sanmiguel,
			.isLargeMesh = true,
			.isGeometryModel = false,
			.meshData = MeshData{},
			.meshFileHeader = MeshFileHeader{},
			.meshDataBuilder = nullptr
		};

	    //meshAssimp.setMeshDataBuilder(new MeshDataBuilderAssimp(8, 1.f));
		meshAssimp.meshFileHeader = loadMeshData(sanmiguelResult.c_str(), meshAssimp.meshData);
		m_meshData = meshAssimp.meshData;
		header = meshAssimp.meshFileHeader;

		m_MeshScene = m_ManagerDevice->GetRenderDevice()->CreateMeshObject(meshAssimp,new MeshDataBuilderAssimp(8,1.2f));
		m_MeshScene = m_ManagerDevice->GetRenderDevice()->CreateMeshObject(meshAssimp);

	}



	void Render() override
	{
		
			float aspectRatio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());
			IFNITY_LOG(LogApp, INFO, "Mesh loaded, ready to render");
		
		

			const mat4 p = glm::perspective(45.0f, aspectRatio, 0.5f, 5000.0f);
			const mat4 view = m_camera.getViewMatrix();

			const PerFrameData perFrameData = { .view = view, .proj = p, .cameraPos = glm::vec4(m_camera.getPosition(), 1.0f) };

			m_ManagerDevice->GetRenderDevice()->WriteBuffer(m_UBO, &perFrameData, sizeof(PerFrameData));

		
			//glDisable(GL_BLEND);

		
			m_MeshScene->DrawIndexed();

			


	
	
	}
	void Animate() override
	{
		IFNITY_LOG(LogApp, INFO, "Animate App");
	}
private:

	void loadMeshAsync(const std::string& filePath)
	{
		// Crear un contexto compartido en el hilo secundario
		GLFWwindow* sharedContextWindow = glfwCreateWindow(1, 1, "", nullptr, glfwGetCurrentContext());
		if(!sharedContextWindow)
		{
			IFNITY_LOG(LogApp, ERROR, "No se pudo crear el contexto compartido de OpenGL.");
			return;
		}

		// Hacer que el contexto compartido sea actual en el hilo secundario
		glfwMakeContextCurrent(sharedContextWindow);

		//
		//MeshObjectDescription meshAssimp =
		//{
		//	.filePath = "",
		//	.isLargeMesh = true,
		//	.isGeometryModel = false,
		//	.meshData = MeshData{},
		//	.meshFileHeader = MeshFileHeader{},
		//	.meshDataBuilder = nullptr
		//};

		////meshAssimp.setMeshDataBuilder(new MeshDataBuilderAssimp(8, 3.f));
		//meshAssimp.meshFileHeader = loadMeshData("data/bistro/Exterior/exterior.obj.meshdata", meshAssimp.meshData);
		//m_meshData = meshAssimp.meshData;
		//header = meshAssimp.meshFileHeader;
		//m_MeshScene = m_ManagerDevice->GetRenderDevice()->CreateMeshObject(meshAssimp);

	

		// Destruir el contexto compartido cuando ya no sea necesario
		glfwDestroyWindow(sharedContextWindow);

	}

	
	MeshFileHeader header;

	MeshData m_meshData;
	BufferHandle m_UBO;
	BufferHandle m_StorageBuffer;
	BufferHandle m_VertexBuffer;
	std::future<void> meshLoadingFuture;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_GraphicsPipeline;
	
	MeshObjectHandle m_MeshScene;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;
	std::shared_ptr<IShader> m_gs;


	// Camera objects 
	IFNITY::EventCameraListener m_CameraListener;
	CameraPositioner_FirstPerson m_camera;


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


	auto api = IFNITY::rhi::GraphicsAPI::OPENGL;


	//return new Source_TestD3D12(api);
	return new Source(api);
}

