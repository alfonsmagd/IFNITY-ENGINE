// IFNITY.cp

#include <Ifnity.h>
#include <stb_image.h>
#include <glad\glad.h>




using namespace IFNITY;
using namespace IFNITY::rhi;

using vec3 = glm::vec3;

class GLTexture
{
public:
	GLTexture(GLenum type, const char* fileName);
	GLTexture(GLenum type, const char* fileName, GLenum clamp);
	GLTexture(GLenum type, int width, int height, GLenum internalFormat);
	GLTexture(int w, int h, const void* img);
	~GLTexture();
	GLTexture(const GLTexture&) = delete;
	GLTexture(GLTexture&&);
	GLenum getType() const { return type_; }
	GLuint getHandle() const { return handle_; }
	GLuint64 getHandleBindless() const { return handleBindless_; }

private:
	GLenum type_ = 0;
	GLuint handle_ = 0;
	GLuint64 handleBindless_ = 0;
};

int getNumMipMapLevels2D(int w, int h)
{
	int levels = 1;
	while ((w | h) >> levels)
		levels += 1;
	return levels;
}

GLTexture::GLTexture(GLenum type, int width, int height, GLenum internalFormat)
	: type_(type)
{
	glCreateTextures(type, 1, &handle_);
	glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, 0);
	glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureStorage2D(handle_, getNumMipMapLevels2D(width, height), internalFormat, width, height);
}

/// Draw a checkerboard on a pre-allocated square RGB image.
uint8_t* genDefaultCheckerboardImage(int* width, int* height)
{
	const int w = 128;
	const int h = 128;

	uint8_t* imgData = (uint8_t*)malloc(w * h * 3); // stbi_load() uses malloc(), so this is safe

	assert(imgData && w > 0 && h > 0);
	assert(w == h);

	if (!imgData || w <= 0 || h <= 0) return nullptr;
	if (w != h) return nullptr;

	for (int i = 0; i < w * h; i++)
	{
		const int row = i / w;
		const int col = i % w;
		imgData[i * 3 + 0] = imgData[i * 3 + 1] = imgData[i * 3 + 2] = 0xFF * ((row + col) % 2);
	}

	if (width) *width = w;
	if (height) *height = h;

	return imgData;
}

GLTexture::GLTexture(GLenum type, const char* fileName)
	: GLTexture(type, fileName, GL_REPEAT)
{}

GLTexture::GLTexture(GLenum type, const char* fileName, GLenum clamp)
	: type_(type)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glCreateTextures(type, 1, &handle_);
	glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, 0);
	glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, clamp);
	glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, clamp);

	const char* ext = strrchr(fileName, '.');

	const bool isKTX = ext && !strcmp(ext, ".ktx");

	switch (type)
	{
		case GL_TEXTURE_2D:
		{
			int w = 0;
			int h = 0;
			int numMipmaps = 0;
			
				uint8_t* img = stbi_load(fileName, &w, &h, nullptr, STBI_rgb_alpha);

				// Note(Anton): replaced assert(img) with a fallback image to prevent crashes with missing files or bad (eg very long) paths.
				if (!img)
				{
					fprintf(stderr, "WARNING: could not load image `%s`, using a fallback.\n", fileName);
					img = genDefaultCheckerboardImage(&w, &h);
					if (!img)
					{
						fprintf(stderr, "FATAL ERROR: out of memory allocating image for fallback texture\n");
						exit(EXIT_FAILURE);
					}
				}

				numMipmaps = getNumMipMapLevels2D(w, h);
				glTextureStorage2D(handle_, numMipmaps, GL_RGBA8, w, h);
				glTextureSubImage2D(handle_, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
				stbi_image_free((void*)img);
			
			glGenerateTextureMipmap(handle_);
			glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, numMipmaps-1);
			glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(handle_, GL_TEXTURE_MAX_ANISOTROPY , 16);
			break;
		
		}
	
	}

	handleBindless_ = glGetTextureHandleARB(handle_);
	glMakeTextureHandleResidentARB(handleBindless_);
}

GLTexture::GLTexture(int w, int h, const void* img)
	: type_(GL_TEXTURE_2D)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glCreateTextures(type_, 1, &handle_);
	int numMipmaps = getNumMipMapLevels2D(w, h);
	glTextureStorage2D(handle_, numMipmaps, GL_RGBA8, w, h);
	glTextureSubImage2D(handle_, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
	glGenerateTextureMipmap(handle_);
	glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, numMipmaps - 1);
	glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(handle_, GL_TEXTURE_MAX_ANISOTROPY, 16);
	handleBindless_ = glGetTextureHandleARB(handle_);
	glMakeTextureHandleResidentARB(handleBindless_);
}

GLTexture::GLTexture(GLTexture&& other)
	: type_(other.type_)
	, handle_(other.handle_)
	, handleBindless_(other.handleBindless_)
{
	other.type_ = 0;
	other.handle_ = 0;
	other.handleBindless_ = 0;
}

GLTexture::~GLTexture()
{
	if (handleBindless_)
		glMakeTextureHandleNonResidentARB(handleBindless_);
	glDeleteTextures(1, &handle_);
}



class GLFramebuffer
{
public:
	GLFramebuffer(int width, int height, GLenum formatColor, GLenum formatDepth);
	~GLFramebuffer();
	GLFramebuffer(const GLFramebuffer&) = delete;
	GLFramebuffer(GLFramebuffer&&) = default;
	GLFramebuffer& operator=(GLFramebuffer&&) = default;
	GLFramebuffer() = default;
	GLuint getHandle() const { return handle_; }
	const GLTexture& getTextureColor() const { return *texColor_.get(); }
	const GLTexture& getTextureDepth() const { return *texDepth_.get(); }
	void bind();
	void unbind();

private:
	int width_;
	int height_;
	GLuint handle_ = 0;

	std::unique_ptr<GLTexture> texColor_;
	std::unique_ptr<GLTexture> texDepth_;
};

GLFramebuffer::GLFramebuffer(int width, int height, GLenum formatColor, GLenum formatDepth)
	: width_(width)
	, height_(height)
{
	glCreateFramebuffers(1, &handle_);

	if (formatColor)
	{
		texColor_ = std::make_unique<GLTexture>(GL_TEXTURE_2D, width, height, formatColor);
		glTextureParameteri(texColor_->getHandle(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(texColor_->getHandle(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0, texColor_->getHandle(), 0);
	}
	if (formatDepth)
	{
		texDepth_ = std::make_unique<GLTexture>(GL_TEXTURE_2D, width, height, formatDepth);
		const GLfloat border[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		//glTextureParameterfv(texDepth_->getHandle(), GL_TEXTURE_BORDER_COLOR, border);
		glTextureParameteri(texDepth_->getHandle(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(texDepth_->getHandle(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glNamedFramebufferTexture(handle_, GL_DEPTH_ATTACHMENT, texDepth_->getHandle(), 0);
	}

	const GLenum status = glCheckNamedFramebufferStatus(handle_, GL_FRAMEBUFFER);

	assert(status == GL_FRAMEBUFFER_COMPLETE);
}

GLFramebuffer::~GLFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &handle_);
}

void GLFramebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, handle_);
	glViewport(0, 0, width_, height_);
}

void GLFramebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


class ExampleLayer: public IFNITY::GLFWEventListener, public IFNITY::Layer
{
public:
	ExampleLayer(): Layer( "Example" ) {}
	~ExampleLayer() {}

	void OnUpdate() override
	{
		IFNITY_LOG( LogApp, INFO, "Update App" );
	}

	void onEventReceived( const IFNITY::WindowResize& event ) override
	{
		IFNITY_LOG( LogApp, WARNING, event.ToString() + "Example Layer" );
	}

	void ConnectToEventBusImpl( void* bus ) override
	{
		auto eventBus = static_cast< IFNITY::GLFWEventSource* >(bus);
		if( eventBus )
		{
			CONNECT_EVENT_LAYER( WindowResize, eventBus );
			CONNECT_EVENT_LAYER( WindowClose, eventBus );
			CONNECT_EVENT_LAYER( KeyPressed, eventBus );
			CONNECT_EVENT_LAYER( KeyRelease, eventBus );
			CONNECT_EVENT_LAYER( MouseMove, eventBus );
			CONNECT_EVENT_LAYER( ScrollMouseMove, eventBus );
			CONNECT_EVENT_LAYER( MouseClick, eventBus );
		}
		else
		{
			// Manage conversion pointer error. 
			IFNITY_LOG( LogApp, ERROR, "The pointer is not type IFNITY::GLFWEventSource*" );
		}
	}
};

class NVMLLayer: public IFNITY::Layer
{
public:
	NVMLLayer(): Layer( "NVML" ) {}
	~NVMLLayer() {}

	void OnUpdate() override
	{
		monitor.refresh();
		monitor.display();
	}

	void ConnectToEventBusImpl( void* bus ) override
	{}

	void OnAttach() override
	{
		loggerDisplayMonitor = LoggerDisplayMonitor();
		monitor.setDisplay( &loggerDisplayMonitor );

		IFNITY_LOG( LogApp, INFO, "NVML Layer is attached" );
	}

private:
	NvmlMonitor monitor;
	LoggerDisplayMonitor loggerDisplayMonitor;
};

class ImGuiTestLayer: public IFNITY::Layer
{
public:
	ImGuiTestLayer(): Layer( "ImGuiTest" ) {}
	~ImGuiTestLayer() {}

	void OnAttach() override
	{
		IFNITY_LOG( LogApp, INFO, "ImGuiTest Layer is attached" );
	}

	void OnUpdate() override
	{
		ImGuiContext* context = GetImGuiContext();
		if( context == nullptr )
		{
			IFNITY_LOG( LogApp, ERROR, "Failed to get ImGui context from DLL" );
			return;
		}
		ImGui::SetCurrentContext( context );

		ChooseApi();
		//IFNITY_LOG(LogApp, INFO, "Update ImGuiTest Layer OnUpdate");
	}

	void ConnectToEventBusImpl( void* bus ) override
	{}

private:
	// A function that is called when the button is clicked
	void AccionPorOpcion( int opcionSeleccionada )
	{
		GraphicsAPI api = IFNITY::App::GetApp().GetGraphicsAPI();
		switch( opcionSeleccionada )
		{
			case 0:
				IFNITY_LOG( LogApp, INFO, "OPENGL" );
				IFNITY::App::GetApp()
					.SetGraphicsAPI( GraphicsAPI::OPENGL, api != GraphicsAPI::OPENGL );
				break;
			case 1:
				IFNITY_LOG( LogApp, INFO, "D3D11" );
				IFNITY::App::GetApp()
					.SetGraphicsAPI( GraphicsAPI::D3D11, api != GraphicsAPI::D3D11 );
				break;
			case 2:
				IFNITY_LOG( LogApp, INFO, "D3D12" );
				IFNITY::App::GetApp()
					.SetGraphicsAPI( GraphicsAPI::D3D12, api != GraphicsAPI::D3D12 );
				break;
			case 3:
				IFNITY_LOG( LogApp, INFO, "VULKAN" );
				IFNITY::App::GetApp()
					.SetGraphicsAPI( GraphicsAPI::VULKAN, api != GraphicsAPI::VULKAN );
				break;
			default:
				break;
		}
	}

	void ChooseApi()
	{
		static int selectOption = 0;
		const char* options[] = { "OPENGL", "D3D11", "D3D12", "VULKAN" };

		ImGui::Begin( "API WINDOW" );

		// Combo box with the options
		if( ImGui::Combo( "Choose Option ", &selectOption, options, IM_ARRAYSIZE( options ) ) )
		{
			// This block executes when a new option is selected
		}

		// Button that triggers the selected action
		if( ImGui::Button( "OK" ) )
		{
			AccionPorOpcion( selectOption );
		}

		ImGui::End();
	}
};

class Source: public IFNITY::App
{

private:
	std::shared_ptr<IShader> m_vs;
	std::shared_ptr<IShader> m_ps;
	
	std::shared_ptr<IShader> m_debugVs;
	std::shared_ptr<IShader> m_debugPs;


	//FPS Counter
	IFNITY::FpsCounter m_FpsCounter;
	float deltaSeconds = 0.0f;
	double timeStamp = 0.0;

	BufferHandle m_vertexBuffer;
	BufferHandle m_indexBuffer;
	GraphicsPipelineHandle m_pipeline;
	GraphicsPipelineHandle m_debugPipeline;
	GraphicsDeviceManager* m_ManagerDevice;

	std::shared_ptr<SimpleRenderer> m_SimpleRenderer;
	std::shared_ptr<DebugRenderer> m_DebugRenderer;
	GLFramebuffer shadowMap;

public:



	Source( IFNITY::rhi::GraphicsAPI api ): IFNITY::App( api ), m_ManagerDevice( IFNITY::App::GetApp().GetDevicePtr() )
	{
		// Push layers including monitoring and GUI
		PushLayer( new IFNITY::NVML_Monitor() );
		PushLayer( new ImGuiTestLayer() );
		PushOverlay( new IFNITY::ImguiLayer() ); // DLL-based ImGui overlay
	}

	//Vertex struct initialization 
	struct VertexData
	{
		vec3 pos;
		vec4 color;
	};

	void Initialize() override
	{
		//File System Instance get it 
		auto& vfs = IFNITY::VFS::GetInstance();
		ShaderCompiler::Initialize();

		vfs.Mount( "Shaders", "Shaders", IFNITY::FolderType::SHADERS );
		vfs.Mount( "test", "Shaders/testShader", IFNITY::FolderType::NO_DEFINED );

		//auto files = vfs.ListFilesInCurrentDirectory("test");
		auto* rdevice = m_ManagerDevice->GetRenderDevice();

		IFNITY_LOG( LogApp, INFO, "START COMPILING INFO  " );

		m_vs = std::make_shared<IShader>();
		m_ps = std::make_shared<IShader>();
		m_debugVs = std::make_shared<IShader>();
		m_debugPs = std::make_shared<IShader>();


		const std::wstring shader = LR"(struct PSInput
		{
		    float4 position : SV_POSITION;
		    float4 color    : COLOR;
		};
		
		PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
		{
		    PSInput result;
		
		    result.position = position;
		    result.color = color;
		
		    return result;
		}
		
		float4 PSMain(PSInput input) : SV_TARGET
		{
		    return input.color;
		}
		)";

		ShaderCreateDescription DescriptionShader;
		{
			DescriptionShader.EntryPoint = L"VSMain";
			DescriptionShader.Profile = L"vs_6_6";
			DescriptionShader.Type = ShaderType::VERTEX_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = shader;
			DescriptionShader.FileName = "vsTriangle";
			DescriptionShader.APIflag = ShaderAPIflag::SPIRV_BIN;
			m_vs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.EntryPoint = L"PSMain";
			DescriptionShader.Profile = L"ps_6_6";
			DescriptionShader.Type = ShaderType::PIXEL_SHADER;
			DescriptionShader.Flags = ShaderCompileFlagType::DEFAULT_FLAG;
			DescriptionShader.ShaderSource = shader;
			DescriptionShader.FileName = "psTriangle";
			DescriptionShader.APIflag = ShaderAPIflag::SPIRV_BIN;
			m_ps->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "quadvert";
			m_debugVs->SetShaderDescription(DescriptionShader);
		}
		{
			DescriptionShader.NoCompile = true;
			DescriptionShader.FileName = "quadfrag";
			m_debugPs->SetShaderDescription(DescriptionShader);
		}

		ShaderCompiler::CompileShader(m_vs.get());
		ShaderCompiler::CompileShader(m_ps.get());
		ShaderCompiler::CompileShader( m_debugVs.get() );
		ShaderCompiler::CompileShader( m_debugPs.get() );


		GraphicsPipelineDescription gdesc;
		{


			//Vertex Attributes Configure 
			rhi::VertexInput vertexInput;
			uint8_t position = 0;
			uint8_t color = 1;
			vertexInput.addVertexAttribute( { .semantic = rhi::VertexSemantic::POSITION,
											.location = position,
											.binding = 0,
											.format = rhi::Format::R32G32B32_FLOAT,
											.offset = offsetof( VertexData,pos ) }, position );

			vertexInput.addVertexAttribute( { .semantic = rhi::VertexSemantic::COLOR,
											.location = color,
											.binding = 0,
											.format = rhi::Format::R32G32B32_FLOAT,
											.offset = offsetof( VertexData,color ) }, color );
			vertexInput.addVertexInputBinding( { .stride = sizeof( VertexData ) }, position );




			gdesc.SetVertexShader( m_vs.get() )
				.SetPixelShader( m_ps.get() )

				.SetVertexInput( vertexInput );

			RasterizationState rasterizationState;
			rasterizationState.cullMode = rhi::CullModeType::None;


			gdesc.SetRasterizationState( rasterizationState );


		}//end of gdesc
		 //Create the pipeline
		m_pipeline = rdevice->CreateGraphicsPipeline( gdesc );
		{
			gdesc.SetVertexShader(m_debugVs.get())
				.SetPixelShader(m_debugPs.get())
				;
			gdesc.AddDebugName( "Debug Pipeline" );
		}
		m_debugPipeline = rdevice->CreateGraphicsPipeline( gdesc );


		//Buffer Data 
		VertexData triangleVertices[] =
		{
			{ { -0.5f, -0.5f , 0.9f }, { 1.0f, 0.0f, 0.0f, 1.0f } },  // rojo (más lejos)
			{ {  0.0f,  0.5f , 0.9f }, { 0.0f, 1.0f, 0.0f, 1.0f } },  // verde (más lejos)
			{ {  0.5f, -0.5f , 0.9f }, { 0.0f, 0.0f, 1.0f, 1.0f } },  // azul (más lejos)

			{ {  0.5f,  0.5f , 0.1f }, { 1.0f, 1.0f, 0.0f, 1.0f } },  // amarillo (más cerca)
			 
			  



		};

		//IndexBuffer
		uint32_t indices[] = {
			0, 1, 2,  // triángulo inferior izquierdo
			2, 1, 3   // triángulo superior derecho
		};



		BufferDescription bufferDesc;
		{
			bufferDesc.SetDebugName( "Vertex Buffer" );
			bufferDesc.SetBufferType( BufferType::VERTEX_BUFFER );
			bufferDesc.SetStorageType( StorageType::DEVICE );
			bufferDesc.SetByteSize( sizeof( triangleVertices ) );
			bufferDesc.SetData( triangleVertices );
			bufferDesc.SetStrideSize( sizeof( VertexData ) );

		}
		m_vertexBuffer = rdevice->CreateBuffer( bufferDesc );

		{
			bufferDesc.SetDebugName( "Index Buffer" );
			bufferDesc.SetBufferType( BufferType::INDEX_BUFFER );
			bufferDesc.SetStorageType( StorageType::HOST_VISIBLE );
			bufferDesc.SetByteSize( sizeof( uint32_t ) * 6 );
			bufferDesc.SetData( indices );
			bufferDesc.SetStrideSize( sizeof( uint32_t ) );
		}
		m_indexBuffer = rdevice->CreateBuffer( bufferDesc );

		//BindPipeline
		m_pipeline->BindPipeline( rdevice );

		//Binding the buffer
		rdevice->BindingIndexBuffer( m_indexBuffer );
		rdevice->BindingVertexAttributesBuffer( m_vertexBuffer );

	

		m_SimpleRenderer = std::make_shared<SimpleRenderer>(m_pipeline);
		m_DebugRenderer = std::make_shared<DebugRenderer>( m_debugPipeline, m_SimpleRenderer.get() );

		m_SimpleRenderer->Initialize( m_ManagerDevice->GetRenderDeviceHandle(),
									  m_ManagerDevice->GetWidth(),
									  m_ManagerDevice->GetHeight());
		m_DebugRenderer->Initialize( m_ManagerDevice->GetRenderDeviceHandle(),
									 m_ManagerDevice->GetWidth(),
									 m_ManagerDevice->GetHeight() );

		rdevice->AddRenderPass( m_SimpleRenderer.get() );
		rdevice->AddRenderPass( m_DebugRenderer.get() );

	}

	void Render() override
	{
		auto* rdevice = m_ManagerDevice->GetRenderDevice();

		//Update FPS
		const double newTimeStamp = App::GetTime();
		deltaSeconds = static_cast< float >(newTimeStamp - timeStamp);
		timeStamp = newTimeStamp;
		m_FpsCounter.tick( deltaSeconds );


		rdevice->StartRecording();


		DrawDescription desc;
		desc.drawMode = DRAW_INDEXED;
		desc.depthTest = true;
		desc.size = 6;

		rdevice->Draw(desc);

	

		rdevice->StopRecording();





	}

	void Animate() override
	{

	}

	~Source() override
	{



	}
};

class Source_TestD3D12: public IFNITY::App
{
public:
	Source_TestD3D12( IFNITY::rhi::GraphicsAPI api ): IFNITY::App( api )
	{
		PushLayer( new ExampleLayer() );
	}
	~Source_TestD3D12() override
	{}
};

IFNITY::App* IFNITY::CreateApp()
{
	auto api = IFNITY::rhi::GraphicsAPI::OPENGL;

	//return new Source_TestD3D12(api);
	return new Source( api );
}

