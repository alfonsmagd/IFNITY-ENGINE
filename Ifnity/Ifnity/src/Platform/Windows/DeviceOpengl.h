#pragma once
#include "Ifnity\GraphicsDeviceManager.h"
#include <Ifnity\Event\WindowEvent.h>
#include <glad\glad.h>



IFNITY_NAMESPACE

class DeviceOpengl final : public GraphicsDeviceManager
{

public:
	virtual ~DeviceOpengl();

	void OnUpdate() override;


	inline unsigned int GetWidth() const override { return m_Props.Width; }
	inline unsigned int GetHeight() const override { return m_Props.Height; }

	


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
	

};

IFNITY_END_NAMESPACE