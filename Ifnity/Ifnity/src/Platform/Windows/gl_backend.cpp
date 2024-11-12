#include "gl_backend.hpp"
#include <GLFW\glfw3.h>



IFNITY_NAMESPACE



namespace OpenGL
{
	void CheckOpenGLError(const char* stmt, const char* fname, int line)
	{
		GLenum err = glGetError();
		if(err != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error " << err << " at " << fname << ":" << line << " - for " << stmt << std::endl;
			exit(1);
		}
	}

	#define GL_CHECK(stmt) do { \
        stmt; \
        CheckOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)



	Device::Device()
	{}
	Device::~Device()
	{}
	void Device::Draw(DrawDescription& desc)
	{

		glViewport(0, 0, 1200, 720);
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
			vShaderFile.open(vsfile);
			fShaderFile.open(psfile);
			std::stringstream vShaderStream, fShaderStream, gShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();

			if(gs)
			{
				const auto& gsfile = gs->GetShaderDescpritionbyAPI(rhi::GraphicsAPI::OPENGL).Filepath;
				gShaderFile.open(gsfile);
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
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
			glNamedBufferSubData(buffer->GetBufferID(), offset, size, data);
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

		const GLuint shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shaderFragment, 1, &fragmentShader, nullptr);
		glCompileShader(shaderFragment);

		glGetShaderiv(shaderFragment, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shaderFragment, 512, NULL, infoLog);
			IFNITY_LOG(LogApp, ERROR, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n", infoLog);
		};

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

		glCreateVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

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
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glUseProgram(program);

		// Create and bind vertex array
		glCreateVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

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