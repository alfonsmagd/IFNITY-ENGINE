#pragma once

#include "pch.h"

#include "Ifnity/Core.h"
#include "Ifnity/Event/Event.h"
#include "Ifnity/Event/WindowEvent.h"




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

	enum API_WINDOW_TYPE
	{
		OPENGL,
		D3D12,
		D3D11,
		VULKAN,

		SIZE_OF_API_WINDOW_TYPE
	};

	//Destructor for the WindowBuilder
	

	//Factory method to create a window
	static Window* Create(API_WINDOW_TYPE api = OPENGL ,const WindowProps& props = WindowProps());

private:

	template<typename WindowType, typename... Args>
	static WindowType* BuildWindow(Args&&... args)
	{
		return new WindowType(std::forward<Args>(args)...);
	}
};



IFNITY_END_NAMESPACE

