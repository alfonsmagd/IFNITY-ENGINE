#include "gl_backend.hpp"
#include <GLFW\glfw3.h>
#include "..\..\..\vendor\glfw\deps\stb_image_write.h"




IFNITY_NAMESPACE



namespace OpenGL
{
	void CheckOpenGLError(const char* stmt, const char* fname, int line)
	{
		GLenum err = glGetError();
		if(err != GL_NO_ERROR)
		{
			IFNITY_LOG(LogApp,ERROR, "OpenGL error " , err ," at " ,fname  ,":" ,line ," - for " , stmt);
			exit(1);
		}
	}

	#define GL_CHECK(stmt) do { \
        stmt; \
        CheckOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)

	std::string readShaderFile(const char* fileName)
	{
		FILE* file = fopen(fileName, "r");

		if(!file)
		{
			IFNITY_LOG(LogCore, ERROR, "I/O error. Cannot open shader file '%s'\n", fileName);
			return std::string();
		}

		fseek(file, 0L, SEEK_END);
		const auto bytesinfile = ftell(file);
		fseek(file, 0L, SEEK_SET);

		char* buffer = (char*)alloca(static_cast<size_t>(bytesinfile) + 1);
		const size_t bytesread = fread(buffer, 1, bytesinfile, file);
		fclose(file);

		buffer[ bytesread ] = 0;

		static constexpr unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };

		if(bytesread > 3)
		{
			if(!memcmp(buffer, BOM, 3))
				memset(buffer, ' ', 3);
		}

		std::string code(buffer);

		while(code.find("#include ") != code.npos)
		{
			const auto pos = code.find("#include ");
			const auto p1 = code.find('<', pos);
			const auto p2 = code.find('>', pos);
			if(p1 == code.npos || p2 == code.npos || p2 <= p1)
			{
				IFNITY_LOG(LogCore, ERROR, "I/O error. Cannot include  shader file '%s'\n", fileName);
				return std::string();
			}
			const std::string name = code.substr(p1 + 1, p2 - p1 - 1);
			const std::string include = readShaderFile(name.c_str());
			code.replace(pos, p2 - pos + 1, include.c_str());
		}

		return code;
	}

	void printShaderSource(const char* text)
	{
		int line = 1;
		IFNITY_LOG(LogCore, INFO, "Shader source code Start : -------------------------------------------------------------\n");
		printf("\n(%3i) ", line);

		while(text && *text++)
		{
			if(*text == '\n')
			{
				printf("\n(%3i) ", ++line);
			}
			else if(*text == '\r')
			{
			}
			else
			{
				printf("%c", *text);
			}
		}

		printf("\n");
		IFNITY_LOG(LogCore, INFO, "Shader source code END : -------------------------------------------------------------\n");
	}

	Device::Device()
	{}
	Device::~Device()
	{}
	void Device::Draw(DrawDescription& desc)
	{

		glViewport(desc.viewPortState.x, 
				   desc.viewPortState.y,
				   desc.viewPortState.width,
				   desc.viewPortState.height);

		SetOpenGLRasterizationState(desc.rasterizationState);
		if(desc.isIndexed)
		{
			glDrawElements(GL_TRIANGLES, desc.size, GL_UNSIGNED_INT, desc.indices);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, desc.size);
		}
		//The next remove this and set in desc. u other size. 



	}

	GraphicsPipelineHandle Device::CreateGraphicsPipeline(GraphicsPipelineDescription& desc)
	{
		auto* vs = desc.vs;
		auto* fs = desc.ps;
		auto* gs = desc.gs;

		if(!vs || !fs)
		{
			IFNITY_LOG(LogApp, WARNING, "Load GetPixelShader or VertexShader");
			return GraphicsPipelineHandle{};
		}

		// 1. retrieve the vertex/fragment/geometry source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			const auto& vsfile = vs->GetShaderDescpritionbyAPI(rhi::GraphicsAPI::OPENGL).Filepath;
			const auto& psfile = fs->GetShaderDescpritionbyAPI(rhi::GraphicsAPI::OPENGL).Filepath;

			vertexCode = readShaderFile(vsfile.c_str());
			fragmentCode = readShaderFile(psfile.c_str());

			if(gs)
			{
				const auto& gsfile = gs->GetShaderDescpritionbyAPI(rhi::GraphicsAPI::OPENGL).Filepath;

				geometryCode = readShaderFile(gsfile.c_str());
			}
		}
		catch(std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		const char* gShaderCode = gs ? geometryCode.c_str() : nullptr;

		// compile shaders 
		GraphicsPipeline* pipeline = new GraphicsPipeline();
		if(gShaderCode)
		{
			pipeline->SetProgram(CreateProgram(vShaderCode, fShaderCode, gShaderCode));
		}
		else
		{
			pipeline->SetProgram(CreateProgram(vShaderCode, fShaderCode));
		}



		return GraphicsPipelineHandle(pipeline);
	}



	BufferHandle Device::CreateBuffer(const BufferDescription& desc)
	{


		if(desc.type == BufferType::CONSTANT_BUFFER)
		{

			const GLsizeiptr kBufferSize = desc.byteSize;


			GLuint perFrameDataBuffer;
			glCreateBuffers(1, &perFrameDataBuffer);
			glNamedBufferStorage(perFrameDataBuffer, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
			//Now I only use a constan buffer in the future proably I will use other types of buffer.
			glBindBufferRange(GL_UNIFORM_BUFFER,
				0, perFrameDataBuffer, 0, kBufferSize);


			Buffer* buffer = new Buffer(perFrameDataBuffer, desc);
			return  BufferHandle(buffer);

		}
		else if(desc.type == BufferType::VERTEX_INDEX_BUFFER)
		{
			return CreateVertexAttAndIndexBuffer(desc);
		}
		else if(desc.type == BufferType::VERTEX_PULLING_BUFFER_INDEX)
		{
			const GLsizeiptr kBufferSize = desc.byteSize;
			// indices
			GLuint dataIndices;
			glCreateBuffers(1, &dataIndices);
			glNamedBufferStorage(dataIndices, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

			glVertexArrayElementBuffer(m_VAO, dataIndices);

			Buffer* buffer = new Buffer(dataIndices, desc);
			return BufferHandle(buffer);
		}
		else if(desc.type == BufferType::VERTEX_PULLING_BUFFER)
		{
			const GLsizeiptr kBufferSize = desc.byteSize;
			// indices
			GLuint dataVertices;
			glCreateBuffers(1, &dataVertices);
			glNamedBufferStorage(dataVertices, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, desc.binding, dataVertices);


			Buffer* buffer = new Buffer(dataVertices, desc);
			return BufferHandle(buffer);
		}
		else
		{
			IFNITY_LOG(LogApp, WARNING, "Buffer type not implemented");
			return nullptr;
		}
	}

	void Device::WriteBuffer(BufferHandle& buffer, const void* data, size_t size, uint32_t offset)
	{

		if(buffer->GetBufferDescription().type == BufferType::CONSTANT_BUFFER)
		{

			//iMPLMENTA AQUI QUE LE MANDO UN
			GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, buffer->GetBufferID()));
			GL_CHECK(glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data));

		}
		else
		{
			GL_CHECK(glNamedBufferSubData(buffer->GetBufferID(), offset, size, data));
		}

	}

	void Device::BindingVertexAttributes(const VertexAttributeDescription* desc, int sizedesc, const void* data, size_t size)
	{

		glBindVertexArray(m_VAO);

		GLuint vertexbuffer;
		glCreateBuffers(1, &vertexbuffer);
		glNamedBufferStorage(vertexbuffer, size, data, 0);
		glVertexArrayVertexBuffer(m_VAO, 0, vertexbuffer, desc[ 0 ].offset, desc[ 0 ].elementStride);
		for(size_t i = 0; i < sizedesc; i++)
		{

			GLuint index = static_cast<GLuint>(desc[ i ].bufferLocation);

			glEnableVertexArrayAttrib(m_VAO, index);

			glVertexArrayAttribFormat(
				m_VAO,
				index,
				desc[ i ].arraySize,
				GL_FLOAT, // Asumiendo que rhi::Format se puede convertir a GLenum
				GL_FALSE, // Asumiendo que no hay normalización
				desc[ i ].offset
			);
			glVertexArrayAttribBinding(m_VAO, index, 0);

			if(desc[ i ].isInstanced)
			{
				glVertexArrayBindingDivisor(m_VAO, index, 1);
			}
		}//end for 

		//glBindVertexArray(0);
	}

	void Device::BindingVertexIndexAttributes(const VertexAttributeDescription* desc, int sizedesc, BufferHandle& bf)
	{

		GLuint indexbuffer = bf->GetBufferID();

		//Check if some vertexattribut have index buffer
		for(size_t i = 0; i < sizedesc; i++)
		{
			if(desc[ i ].haveIndexBuffer)
			{
				glVertexArrayVertexBuffer(m_VAO, 0, indexbuffer, desc[ i ].offset, desc[ i ].elementStride);
			}
			else
			{
				glEnableVertexArrayAttrib(m_VAO, desc[ i ].bufferLocation);
				glVertexArrayAttribFormat
				(
					m_VAO,
					desc[ i ].bufferLocation,
					desc[ i ].arraySize,
					GL_FLOAT,
					GL_FALSE,
					desc[ i ].offset
				);
				glVertexArrayAttribBinding(m_VAO, desc[ i ].bufferLocation, 0);
			}
		}//endfor
	}

	/**
	 * @brief Creates a texture in OpenGL from the given description.
	 *
	 * This function loads a texture from an image file specified in the description,
	 * creates an OpenGL texture object, sets its parameters, and uploads the image data to it.
	 *
	 * @param desc The description of the texture to be created, including its format, dimensions, and wrapping mode.
	 * @return TextureHandle A handle to the created texture.
	 */
	TextureHandle Device::CreateTexture(TextureDescription& desc)
	{
		//Create Switch for the type of texture
		switch(desc.dimension)
		{
		case rhi::TextureDimension::TEXTURE2D:
			return CreateTexture2DImpl(desc);
			break;

		case rhi::TextureDimension::TEXTURECUBE:
			return CreateTextureCubeMapImpl(desc);
			break;
		default:
			IFNITY_LOG(LogApp, ERROR, "Texture dimension not implemented");
			return nullptr;
		}

	}




	GLuint Device::CreateVAO()
	{
		// Create and bind VAO object
		GLuint vao;
		glCreateVertexArrays(1, &vao);
		glBindVertexArray(vao);

		return vao;
	}

	TextureHandle Device::CreateTexture2DImpl(TextureDescription& desc)
	{
		// Load texture from image file
		const void* img = LoadTextureFromFileDescription(desc);
		if(!img)
		{
			IFNITY_LOG(LogApp, ERROR, "Failed to load texture from file.");
			return nullptr;
		}

		GLenum format = OpenGL::c_FormatMap[ (uint8_t)desc.format ].glFormat;

		GLuint textureID;
		glCreateTextures(ConvertToOpenGLTextureTarget(desc.dimension), 1, &textureID);
		if(textureID == 0)
		{
			IFNITY_LOG(LogApp, ERROR, "Failed to create texture target in OpenGL");
			FreeTexture(img);
			return nullptr;
		}

		glTextureParameteri(textureID, GL_TEXTURE_MAX_LEVEL, 0);
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, ConvertToOpenGLTextureWrapping(desc.wrapping));
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, ConvertToOpenGLTextureWrapping(desc.wrapping));
		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureStorage2D(textureID, 1, format, desc.width, desc.height);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureSubImage2D(textureID, 0, 0, 0, desc.width, desc.height, GL_RGB, GL_UNSIGNED_BYTE, img);
		glBindTextures(0, 1, &textureID);

		// Free the image
		FreeTexture(img);

		ITexture* texture = new Texture(desc, textureID);
		return TextureHandle(texture);
	}

	TextureHandle Device::CreateTextureCubeMapImpl(TextureDescription& desc)
	{
		// Load texture from image file
		const void* img = LoadTextureFromFileDescription(desc);
		if(!img)
		{
			IFNITY_LOG(LogApp, ERROR, "Failed to load texture from file.");
			return nullptr;
		}

		//Generate Bitmap to elaborate CubeMap implementation
		Bitmap in(desc.width, desc.height, desc.comp, rhi::eBitmapFormat_Float, img);
		FreeTexture(img); // Free the image


		Bitmap out = convertEquirectangularMapToVerticalCross(in);


		Bitmap cubemap = convertVerticalCrossToCubeMapFaces(out);

		GLenum format = OpenGL::c_FormatMap[ (uint8_t)desc.format ].glFormat;
		GLenum textureWrap = ConvertToOpenGLTextureWrapping(desc.wrapping);

		stbi_write_hdr("screenshot.hdr", out.w_, out.h_, out.comp_, (const float*)out.data_.data());

		GLuint cubemapTex;
		{
			glCreateTextures(ConvertToOpenGLTextureTarget(desc.dimension), 1, &cubemapTex);
			glTextureParameteri(cubemapTex, GL_TEXTURE_WRAP_S, textureWrap);
			glTextureParameteri(cubemapTex, GL_TEXTURE_WRAP_T, textureWrap);
			glTextureParameteri(cubemapTex, GL_TEXTURE_WRAP_R, textureWrap);
			glTextureParameteri(cubemapTex, GL_TEXTURE_BASE_LEVEL, 0);
			glTextureParameteri(cubemapTex, GL_TEXTURE_MAX_LEVEL, 0);
			glTextureParameteri(cubemapTex, GL_TEXTURE_MAX_LEVEL, 0);
			glTextureParameteri(cubemapTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(cubemapTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureStorage2D(cubemapTex, 1, format, cubemap.w_, cubemap.h_);
			const uint8_t* data = cubemap.data_.data();

			for(unsigned i = 0; i != 6; ++i)
			{
				glTextureSubImage3D(cubemapTex, 0, 0, 0, i, cubemap.w_, cubemap.h_, 1, GL_RGB, GL_FLOAT, data);
				data += cubemap.w_ * cubemap.h_ * cubemap.comp_ * Bitmap::getBytesPerComponent(cubemap.fmt_);
			}
			glBindTextures(1, 1, &cubemapTex);
		}

		ITexture* texture = new Texture(desc, cubemapTex);
		return TextureHandle(texture);


	}

	/**
	 * @brief Creates an OpenGL program from vertex and fragment shader source code.
	 *
	 * @param vertexShader The source code of the vertex shader.
	 * @param fragmentShader The source code of the fragment shader.
	 * @return Program The created OpenGL program.
	 */
	Program Device::CreateProgram(const char* vertexShader, const char* fragmentShader)
	{
		int success;
		char infoLog[ 512 ];

		// Complete the shader program
		const GLuint shaderVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shaderVertex, 1, &vertexShader, nullptr);
		glCompileShader(shaderVertex);

		// Print compile errors if any
		glGetShaderiv(shaderVertex, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shaderVertex, 512, NULL, infoLog);
			IFNITY_LOG(LogApp, ERROR, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n", infoLog);
		};
		#if _DEBUG
		printShaderSource(vertexShader);
		#endif

		const GLuint shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shaderFragment, 1, &fragmentShader, nullptr);
		glCompileShader(shaderFragment);

		glGetShaderiv(shaderFragment, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shaderFragment, 512, NULL, infoLog);
			IFNITY_LOG(LogApp, ERROR, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n", infoLog);
		};

		#if _DEBUG
		printShaderSource(fragmentShader);
		#endif
		const GLuint program = glCreateProgram();
		glAttachShader(program, shaderVertex);
		glAttachShader(program, shaderFragment);

		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glUseProgram(program);

		GetMeshVAO("default");

		m_Program.id = program;

		return m_Program;
	}


	/**
	 * @brief Creates an OpenGL program from vertex and fragment shader source code.
	 *
	 * @param vertexShader The source code of the vertex shader.
	 * @param fragmentShader The source code of the fragment shader.
	 * @return Program The created OpenGL program.
	 */
	Program Device::CreateProgram(const char* vertexShader, const char* fragmentShader, const char* geometryShader)
	{
		int success;
		char infoLog[ 512 ];

		// Compile vertex shader
		const GLuint shaderVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shaderVertex, 1, &vertexShader, nullptr);
		glCompileShader(shaderVertex);
		glGetShaderiv(shaderVertex, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shaderVertex, 512, NULL, infoLog);
			IFNITY_LOG(LogApp, ERROR, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n", infoLog);
		}
		#if _DEBUG
		printShaderSource(vertexShader);
		#endif

		// Compile fragment shader
		const GLuint shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shaderFragment, 1, &fragmentShader, nullptr);
		glCompileShader(shaderFragment);
		glGetShaderiv(shaderFragment, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shaderFragment, 512, NULL, infoLog);
			IFNITY_LOG(LogApp, ERROR, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n", infoLog);
		}
		#if _DEBUG
		printShaderSource(fragmentShader);
		#endif

		// Compile geometry shader if provided
		GLuint shaderGeometry = 0;
		if(geometryShader)
		{
			shaderGeometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(shaderGeometry, 1, &geometryShader, nullptr);
			glCompileShader(shaderGeometry);
			glGetShaderiv(shaderGeometry, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				glGetShaderInfoLog(shaderGeometry, 512, NULL, infoLog);
				IFNITY_LOG(LogApp, ERROR, "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n", infoLog);
			}
			#if _DEBUG
			printShaderSource(geometryShader);
			#endif
		}

		// Create program and attach shaders
		const GLuint program = glCreateProgram();
		glAttachShader(program, shaderVertex);
		glAttachShader(program, shaderFragment);
		if(geometryShader)
		{
			glAttachShader(program, shaderGeometry);
		}
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			IFNITY_LOG(LogCore, ERROR, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n", infoLog);
		}
		glUseProgram(program);

		// Create and bind vertex array

		GetMeshVAO("default");

		m_Program.id = program;

		return m_Program;
	}


	/**
	* @brief Creates a vertex attribute and index buffer in OpenGL.
	*
	* This function creates a buffer for vertex attributes and indices, allocates storage for it,
	* and binds it as an element buffer to the vertex array object (VAO) this Uses DSA OpenGL4.5.
	*
	* @param desc The description of the buffer to be created, including its size and type.
	* @return BufferHandle A handle to the created buffer.
	*/
	BufferHandle Device::CreateVertexAttAndIndexBuffer(const BufferDescription& desc)
	{
		const GLsizeiptr kBufferSize = desc.byteSize;

		//Create the buffer id 
		GLuint meshData;
		glCreateBuffers(1, &meshData);

		//Create the buffer storage
		glNamedBufferStorage(meshData, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayElementBuffer(m_VAO, meshData); // index buffer

		Buffer* buffer = new Buffer(meshData, desc);

		return BufferHandle(buffer);

	}

	void Device::GetMeshVAO(const std::string mesh)
	{	
		if(m_MeshVAOs.find(mesh) == m_MeshVAOs.end())
		{
			GLuint vao;
			glCreateVertexArrays(1, &vao);
			m_MeshVAOs[ mesh ] = vao;

			if(mesh == "default")
			{
				m_VAO = vao;
			}
			
		}
		glBindVertexArray(m_MeshVAOs[ mesh ]);
	}



	/**
	* @brief Destructor for the GraphicsPipeline class.
	*
	* This destructor deletes the OpenGL program associated with the graphics pipeline
	* if the program ID is not zero.
	*/
	GraphicsPipeline::~GraphicsPipeline()
	{
		if(m_Program.id != 0)
		{
			glDeleteProgram(m_Program.id);
		}
	}

};

IFNITY_END_NAMESPACE