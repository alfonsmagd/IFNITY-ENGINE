#pragma once
#include "Ifnity\GraphicsDeviceManager.h"
#include <Ifnity\Event\WindowEvent.h>
#include <glad\glad.h>



IFNITY_NAMESPACE

class DeviceOpengl final : public GraphicsDeviceManager
{

public:
	DeviceOpengl(const WindowProps& props);
	virtual ~DeviceOpengl();

	void OnUpdate() override;


	inline unsigned int GetWidth() const override { return m_WindowData.props.Width; }
	inline unsigned int GetHeight() const override { return m_WindowData.props.Height; }

	


protected:
	// Window attributes
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;
	bool InitInternalInstance() override { return true; } //TODO: Implement this function in .cpp file.
	bool ConfigureSpecificHintsGLFW() const  override;
	bool CreateAPISurface() override;
private:
	void Init();
	void InitializeGLAD();
	std::string GetOpenGLInfo();

private:
	// Associate GLFW window with this class with unique pointer
	// This is a unique pointer because we want to make sure that there is only one window

	//Create a EventSource for the WindowGroup

	// Struct to hold window data
	struct WindowData
	{
		std::string Title;
		WindowProps props;
		bool VSync;

		GLFWEventSource  GLFWEventSourceBus;
	}m_WindowData;

};

IFNITY_END_NAMESPACE