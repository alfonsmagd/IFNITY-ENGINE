#pragma once
#include "Ifnity\GraphicsDeviceManager.hpp"
#include <Ifnity\Event\WindowEvent.h>
#include "gl_backend.hpp"
#include <glad\glad.h>



IFNITY_NAMESPACE

//forward declaration

//and utils simplification types 

using DeviceHandle = std::shared_ptr<IDevice>;

class IShader;

class DeviceOpengl final : public GraphicsDeviceManager
{

public:
	  ~DeviceOpengl() override;

	void OnUpdate() override;


	inline unsigned int GetWidth() const override { return m_Props.Width; }
	inline unsigned int GetHeight() const override { return m_Props.Height; }
	static void DemoTriangle(const char* sv, const char* sp);
	void RenderDemo(int w, int h) const override;
	
	IDevice* GetRenderDevice() const override { return m_RenderDevice.get(); }


protected:
	// Window attributes
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;
	bool InitInternalInstance() override { return true; } //TODO: Implement this function in .cpp file.
	bool ConfigureSpecificHintsGLFW() const  override;
	bool InitializeDeviceAndContext() override;
	void ResizeSwapChain() override;
	void InitializeGui() override;
	void Shutdown() override;
	void InternalPreDestroy() override;
	void LoadAppPipelineDescription() override; //TODO: ABSOLUTE. 
	void ClearBackBuffer(float* color) override;
private:
	void Init();
	void InitializeGLAD();
	void BuildGraphicsShaders();
	void CompileGraphicsShader(const char* vertexShader, const char* fragmentShader);
	void UseShader();
	std::string GetOpenGLInfo() const ;

	template<typename RenderCallback, typename... Args>
	void Draw(RenderCallback renderCallback, Args&&... args);
	
	
	


private:
	// Associate GLFW window with this class with unique pointer
	// This is a unique pointer because we want to make sure that there is only one window

	//Create a EventSource for the WindowGroup

	// Struct to hold window data
	
	float m_Color[4] = { 1.0f,1.0f,1.0f,1.0f };
	GLuint m_VAO;
	// Shader
	DeviceHandle m_RenderDevice;

};

IFNITY_END_NAMESPACE