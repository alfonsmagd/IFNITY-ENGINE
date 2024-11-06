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

	GraphicsPipeline Device::CreateGraphicsPipeline(GraphicsPipelineDescription& desc)
	{

		auto* vs = desc.vs;
		auto* fs = desc.ps;

		if(!vs || !fs)
		{
			IFNITY_LOG(LogApp, WARNING, "Load GetPixelShader or VertexShader");
			return GraphicsPipeline{};
		}

		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		//This will be removed now its based in OpenGl tutorial 
		try
		{
			const auto& vsfile = vs->GetShaderDescpritionbyAPI(rhi::GraphicsAPI::OPENGL).Filepath;
			const auto& psfile = fs->GetShaderDescpritionbyAPI(rhi::GraphicsAPI::OPENGL).Filepath;
			vShaderFile.open(vsfile);
			fShaderFile.open(psfile);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch(std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		//
		// compile shaders 
		GraphicsPipeline pipeline{};
		pipeline.program = CreateProgram(vShaderCode, fShaderCode);

		return pipeline;



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
		if(desc.type == BufferType::VERTEX_INDEX_BUFFER)
		{
			return CreateVertexAttAndIndexBuffer(desc);
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




};

IFNITY_END_NAMESPACE