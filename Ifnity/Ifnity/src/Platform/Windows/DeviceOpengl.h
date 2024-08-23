#pragma once
#include "Ifnity\GraphicsDeviceManager.hpp"
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
	static void DemoTriangle(const char* sv, const char* sp);
	void RenderDemo(int w, int h) const override;


protected:
	// Window attributes
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;
	bool InitInternalInstance() override { return true; } //TODO: Implement this function in .cpp file.
	bool ConfigureSpecificHintsGLFW() const  override;
	bool InitializeDeviceAndContext() override;
	void ResizeSwapChain() override;
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