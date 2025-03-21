

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
		if(context == nullptr)
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
	TextureHandle m_depth;
	GraphicsDeviceManager* m_ManagerDevice;
	GraphicsPipelineHandle m_SolidPipeline;
	GraphicsPipelineHandle m_WireFramePipeline;
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;

	uint32_t msizeIndex = 0;	
	ImGuiTestLayer* m_ImGuiLayer;
public:
	Source(IFNITY::rhi::GraphicsAPI api): IFNITY::App(api), m_ManagerDevice(IFNITY::App::GetApp().GetDevicePtr())
	{
		// Obtener el contexto de ImGui desde IFNITY  DLL
		/*ImGuiContext* context = GetImGuiContext();
		if (context == nullptr)
		{
		IFNITY_LOG(LogApp, ERROR, "Failed to get ImGui context from DLL");
		return;
		}*/

		// Establecer el contexto de ImGui en la aplicación principal
		//ImGui::SetCurrentContext(context);
		m_ImGuiLayer = new ImGuiTestLayer();
		PushLayer(new   IFNITY::NVML_Monitor());
		PushLayer(m_ImGuiLayer);
		PushOverlay(new IFNITY::ImguiLayer()); //Capa de dll 



	}

	void Initialize() override
	{
		//File System Instance get it 
		auto& vfs = IFNITY::VFS::GetInstance();

		vfs.Mount("Shaders", "Shaders", IFNITY::FolderType::SHADERS);
		vfs.Mount("test", "Shaders/testShader", IFNITY::FolderType::NO_DEFINED);

		auto files = vfs.ListFilesInCurrentDirectory("test");
		auto* rdevice = m_ManagerDevice->GetRenderDevice();
		auto getFileName = [ &files ](const std::string& extension) -> const char*
			{
				for(const auto& file : files)
				{
					if(file.size() >= extension.size() &&
					   file.compare(file.size() - extension.size(), extension.size(), extension) == 0)
					{
						return file.c_str();
					}
				}
				return nullptr;
			};

		IFNITY::testShaderCompilation(getFileName(".vert"), "Shaders/testShader/main.vert.spv");
		IFNITY::testShaderCompilation(getFileName(".frag"), "Shaders/testShader/main.frag.spv");

		IFNITY_LOG(LogApp, INFO, "END APP ONLY TEST SHADER BUILDING ");



		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.NoCompile = true;
			//DescriptionShader.FileName = "triangle01.vert";
			DescriptionShader.FileName = "texture_simple.vert";
			
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			//DescriptionShader.FileName = "triangle01.frag";
			
			DescriptionShader.FileName = "texture_simple.frag";
			m_ps->SetShaderDescription(DescriptionShader);
		}

		ShaderCompiler::CompileShader(m_vs.get());
		ShaderCompiler::CompileShader(m_ps.get());

		const uint32_t wireframe = 1;
		GraphicsPipelineDescription gdesc1;
		

		{
			gdesc1.AddDebugName("Texture ").
				SetVertexShader(m_vs.get()).
				SetPixelShader(m_ps.get()).
				SetRasterizationState({ 
				.primitiveType = rhi::PrimitiveType::TriangleStrip,
				.cullMode = rhi::CullModeType::Back ,
				.polygonMode = rhi::PolygonModeType::Fill }).
				SetRenderState({ .depthTest = false });


			m_SolidPipeline = rdevice->CreateGraphicsPipeline(gdesc1);
		}

		m_SolidPipeline->BindPipeline(rdevice);



		BufferDescription desc;
		desc.size = sizeof(glm::mat4);
		desc.type = BufferType::CONSTANT_BUFFER;
		desc.binding = 0;
		desc.data = nullptr;

		m_UBO = rdevice->CreateBuffer(desc);

	
		int w, h, comp;
		const uint8_t* img = stbi_load("data/wood.jpg", &w, &h, &comp, 4);


		////DepthText texture
		//TextureDescription descTexture;
		//descTexture.dimensions = {(uint32_t)w, (uint32_t)h};
		//descTexture.format = Format::RGBA_UNORM8;
		//descTexture.usage = TextureUsageBits::SAMPLED;
		//descTexture.isDepth = false;
		//descTexture.debugName = "Depth buffer";
		//descTexture.data = img;

		////DepthStencil texture
		//m_depth = rdevice->CreateTexture(descTexture);
		

		




	


		//exit(0);

	}

	void Render() override
	{
		auto* rdevice = m_ManagerDevice->GetRenderDevice();


		float ratio = m_ManagerDevice->GetWidth() / static_cast<float>(m_ManagerDevice->GetHeight());

		const mat4 m = glm::rotate(mat4(1.0f), (float)glfwGetTime(), vec3(0.0f, 0.0f, 1.0f));
		const mat4 p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);

		const struct PerFrameData {
			mat4 mvp;
			uint32_t textureId;
		} pc = {
			.mvp       = p * m,
			//.textureId = texture.index(),
		};

		//StartRecording
		rdevice->StartRecording();
		rdevice->WriteBuffer(m_UBO, glm::value_ptr(pc.mvp), sizeof(glm::mat4));
		DrawDescription desc;
		//First Draw solid line pipeline 
		{
			desc.size = 6;
			desc.drawMode = DRAW;
		
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

