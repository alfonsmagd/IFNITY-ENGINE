#pragma once
#include "Ifnity\Window.h"

#include <GLFW\glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW\glfw3native.h>


IFNITY_NAMESPACE

class WindowOpengl final: public Window
{

public:
	WindowOpengl(const WindowProps& props);
	virtual ~WindowOpengl();

	void OnUpdate() override;
	

	unsigned int GetWidth() const override { return m_WindowData.props.Width; }
	unsigned int GetHeight() const override { return m_WindowData.props.Width; }

	// Window attributes
	void SetEventCallback(const EventCallbackFn& callback) override { m_WindowData.EventCallback = callback; }
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	//TODO: Implement this function
	void* GetNativeWindow() const override { return nullptr; }

private:
	void Init();
	void Shutdown();

private:
	// Associate GLFW window with this class with unique pointer
	// This is a unique pointer because we want to make sure that there is only one window
	
	GLFWwindow* m_Window;
	// Struct to hold window data
	struct WindowData
	{
		std::string Title;
		WindowProps props;
		bool VSync;

		EventCallbackFn EventCallback;
	}m_WindowData;

};

IFNITY_END_NAMESPACE