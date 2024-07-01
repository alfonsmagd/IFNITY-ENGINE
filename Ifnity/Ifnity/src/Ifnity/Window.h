#pragma once

#include "pch.h"

#include "Ifnity/Core.h"
#include "Ifnity/Event/Event.h"


IFNITY_NAMESPACE

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

// Interface representing a desktop system based Window
// This is a pure virtual class, it has no implementation.
class IFNITY_API Window
{
public:
	using EventCallbackFn = std::function<void(Event&)>;

	virtual ~Window() = default;

	virtual void OnUpdate() = 0;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;

	// Window attributes
	virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;

	virtual void* GetNativeWindow() const = 0;

	static Window* Create(const WindowProps& props = WindowProps());
};


IFNITY_END_NAMESPACE
