#pragma once

#include "pch.h"

#include "Ifnity/Core.h"
#include "Ifnity/Event/Event.h"
#include "Ifnity/Event/WindowEvent.h"
#include "Ifnity/Graphics/ifrhi.h"




IFNITY_NAMESPACE


class Window;

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
};


class IFNITY_API Window
{
public:

	virtual ~Window() = default;

	virtual void OnUpdate() = 0;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;

	// Window attributes
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;
	virtual GLFWEventSource* GetGLFWEventSource() = 0;
	virtual void* GetNativeWindow() const = 0;

	//Base Methods to build in glfw window process with no API specified by default. 
	void CreateWindowSurface(const WindowProps& props);
	bool CreateInstance();

	//Destructor for the WindowBuilder


	//Factory method to create a window
	static Window* Create(rhi::GraphicsAPI api = rhi::GraphicsAPI::OPENGL, const WindowProps& props = WindowProps());

protected:
	// Api Device specific methods interface to be implemented by the derived class.
	virtual bool InitInternalInstance() = 0;

protected:
	WindowProps m_Props;
	bool m_InstanceCreated = false;

private:

	template<typename WindowType, typename... Args>
	static WindowType* BuildWindow(Args&&... args)
	{
		return new WindowType(std::forward<Args>(args)...);
	}

private:
	bool m_isNvidia = true;
	rhi::GraphicsAPI m_API{ rhi::GraphicsAPI::OPENGL }; // By default opengl is the api 
};



IFNITY_END_NAMESPACE

