#pragma once

#include "pch.h"

#include "Ifnity/Core.h"
#include "Ifnity/Event/Event.h"
#include "Ifnity/Event/WindowEvent.h"

#define GLFW_INCLUDE_NONE // Do not include any OpenGL headers
#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif // _WIN32
#include <GLFW/glfw3native.h>

#include "Ifnity/Graphics/ifrhi.h"




IFNITY_NAMESPACE


class GraphicsDeviceManager;

struct WindowProps
{
	std::string Title;
	unsigned int Width;
	unsigned int Height;

	WindowProps(const std::string& title = "Ifnity Engine",
		unsigned int width = 1280,
		unsigned int height = 720)
		: Title(title), Width(width), Height(height)
	{
	}

	bool VSync = false;
	GLFWEventSource GLFWEventSourceBus;
};


class IFNITY_API GraphicsDeviceManager
{
public:

	virtual ~GraphicsDeviceManager() = default;

	virtual void OnUpdate() = 0;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;

	// GraphicsDeviceManager attributes
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;
	
	virtual void* GetNativeWindow() const = 0;

	//Base Methods to build in glfw window process with no API specified by default. 
	bool CreateWindowSurface(const WindowProps& props);
	bool CreateInstance();
	//Get GLFWEventSourceBus to connect Listeners
	GLFWEventSource* GetGLFWEventSource()  { return &m_Props.GLFWEventSourceBus; }
	
	//Destructor for the WindowBuilder


	//Factory method to create a window
	static GraphicsDeviceManager* Create(rhi::GraphicsAPI api = rhi::GraphicsAPI::OPENGL, const WindowProps& props = WindowProps());

protected:
	// Api Device specific methods interface to be implemented by the derived class.
	virtual bool InitInternalInstance() = 0;
	virtual bool CreateAPISurface() = 0;
	virtual bool ConfigureSpecificHintsGLFW()  const = 0;

protected:
	WindowProps m_Props;
	bool m_IsNvidia = false;
	GLFWwindow* m_Window = nullptr;
	bool m_InstanceCreated = false;

private:
	void SetGLFWCallbacks();
	template<typename WindowType, typename... Args>
	static WindowType* BuildWindow(Args&&... args)
	{
		return new WindowType(std::forward<Args>(args)...);
	}

private:
	
	rhi::GraphicsAPI m_API{ rhi::GraphicsAPI::OPENGL }; // By default opengl is the api 
};



IFNITY_END_NAMESPACE

