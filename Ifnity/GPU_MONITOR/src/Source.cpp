

// IFNITY.cp

#include <Ifnity.h>



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

		ChooseApi();
		ShowColorPiceckerWindow();
		//IFNITY_LOG(LogApp, INFO, "Update ImGuiTest Layer OnUpdate");
	}
	// Heredado v�a Layer
	void ConnectToEventBusImpl(void* bus) override
	{
	}
private:
	// Una funci�n que se llama al hacer clic en el bot�n
	void AccionPorOpcion(int opcionSeleccionada) {

		GraphicsAPI api = IFNITY::App::GetApp().GetGraphicsAPI();
		switch (opcionSeleccionada) {
		case 0:
			// Acci�n para la opci�n 1
			IFNITY_LOG(LogApp, INFO, "OPENGL");
			

			IFNITY::App::GetApp()
				.SetGraphicsAPI(GraphicsAPI::OPENGL,api != GraphicsAPI::OPENGL);
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

	void ChooseApi() {
		static int selectOption = 0;
		const char* options[] = { "OPENGL", "D3D11","D3D12","VULKAN"};

		ImGui::Begin("API WINDOW");  // Comienza la creaci�n de la ventana

		// Combo box con las opciones
		if (ImGui::Combo("Choose Option ", &selectOption, options, IM_ARRAYSIZE(options))) {
			// Este bloque se ejecuta cada vez que se selecciona una opci�n diferente
		}

		// Bot�n que ejecuta la funci�n cuando se hace clic
		if (ImGui::Button("OK")) {
			AccionPorOpcion(selectOption);
		}

		ImGui::End();  // Termina la creaci�n de la ventana
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

	void Initialize() override
	{

	}
	void Render() override
	{
		
	}
	void Animate()override
	{

	}
	~Source() override {}
};


IFNITY::App* IFNITY::CreateApp()
{


	auto api = IFNITY::rhi::GraphicsAPI::OPENGL;

	
	//return new Source_TestD3D12(api);
	return new Source(api);
}

