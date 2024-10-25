#pragma once

#include "pch.h"
#include "Ifnity/Event/WindowEvent.h"
#include "Ifnity\Graphics\IShader.hpp"
#include "Ifnity\Graphics\Interfaces\IDevice.hpp"

// GRAPHIC API
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE // Do not include any OpenGL headers
#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32



#endif // _WIN32
#include <GLFW/glfw3native.h>

#include "Ifnity/Graphics/ifrhi.h"

#ifdef _WIN32
#include <wrl/client.h> //Handle COM objects.
//D3D11
#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_3.h>
#include <dxgidebug.h>

//D3D12
#include <d3d12.h>
#include <dxgi1_4.h>
#include "Platform/Windows/d3dx12.h"
#endif // _WIN32

IFNITY_NAMESPACE


class GraphicsDeviceManager;

struct WindowData
{
	std::string Title;
	unsigned int Width;
	unsigned int Height;

	WindowData(const std::string& title = "Ifnity Engine",
		unsigned int width = 1280,
		unsigned int height = 720)
		: Title(title), Width(width), Height(height)
	{
	}

	bool VSync = false;
	GLFWEventSource GLFWEventSourceBus;
};


// Define the possible states of the graphics device
enum class StateGraphicsDevice : uint8_t
{
	INITIALIZED,         // The graphics device has been initialized
	NOT_INITIALIZED,     // The graphics device has not been initialized
	CHANGING_API,        // The graphics device is changing its API
	API_CHANGED,         // The API change has been completed
	API_NOT_CHANGED,     // The API change has not been performed
	API_NOT_SUPPORTED,   // The requested API is not supported
	API_NOT_FOUND        // The requested API was not found
};



class IFNITY_API GraphicsDeviceManager
{
public:

	GraphicsDeviceManager() = default;
	virtual ~GraphicsDeviceManager() = default;

	virtual void OnUpdate() = 0;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;
	//To implment next.
	[[nodiscard]] virtual IDevice* GetRenderDevice() const { return nullptr; }

	//Base Methods to build in glfw window process with no API specified by default. 
	bool CreateWindowSurface(const WindowData&& props);
	bool CreateInstance();
	//Base Methods virtual 
	virtual void Shutdown();
	virtual void RenderDemo(int w, int h) const; // Its a function to do test fast, its not part of solution now. 
	//Get GLFWEventSourceBus to connect Listeners
	GLFWEventSource* GetGLFWEventSource() { return &m_Props.GLFWEventSourceBus; }


	void InitImGui();

	//Factory method to create a window
	static GraphicsDeviceManager* Create(rhi::GraphicsAPI api = rhi::GraphicsAPI::D3D11);
	// Método estático para obtener la API gráfica
	static rhi::GraphicsAPI GetStaticGraphicsAPI() { return g_API; }
	

	virtual void* Wrapper_ptr_data() { return 0; };
	virtual void ClearBackBuffer(float* color) {};
	virtual void LoadAppPipelineDescription() {}; //TODO: ABSOLUTE. 

	


	//APP user 
	void LoadAppShaders(IShader* vs, IShader* ps) {
		m_VS = vs; 
		m_PS = ps; }
	const IShader* GetVertexShader() { return m_VS; }
	const IShader* GetPixelShader() {  return m_PS; }

protected:
	// Api Device specific methods interface to be implemented by the derived class.
	virtual bool InitInternalInstance() = 0;
	virtual bool InitializeDeviceAndContext() = 0;
	virtual bool ConfigureSpecificHintsGLFW()  const = 0;
	// GraphicsDeviceManager attributes
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;
	virtual void ResizeSwapChain() = 0;
	virtual void InitializeGui() = 0;
	virtual void InternalPreDestroy() = 0; 
	
	
	

	WindowData m_Props;
	bool m_IsNvidia = false;
	GLFWwindow* m_Window = nullptr;
	bool m_InstanceCreated = false;
	
	//Graphics Shaders
	

private:
	void SetGLFWCallbacks();
	void SetGraphicsDeviceState(StateGraphicsDevice state) { m_StateGraphicsDevice = state; }
	void SetWindowIcon();
	template<typename WindowType, typename... Args>
	static WindowType* BuildWindow(Args&&... args)
	{
		return new WindowType(std::forward<Args>(args)...);
	}


	
	static rhi::GraphicsAPI g_API; // By default opengl is the api 
	StateGraphicsDevice m_StateGraphicsDevice{ StateGraphicsDevice::NOT_INITIALIZED };

	IShader* m_VS = nullptr;
	IShader* m_PS = nullptr;
};



IFNITY_END_NAMESPACE

