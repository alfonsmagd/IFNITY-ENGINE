// File: Utils.cpp
// This header file is built to define utility functions and classes for the Graphics module.


#include "Utils.hpp"
#include "Ifnity\Graphics\Interfaces\ITexture.hpp"


IFNITY_NAMESPACE

namespace Utils
{

	BufferDescription CreateConstantBufferDescription(uint64_t byteSize, const std::string& debugName)
	{

		return BufferDescription{}.SetByteSize(byteSize)
			.SetDebugName(debugName)
			.SetBufferType(BufferType::CONSTANT_BUFFER);
	}

	IFNITY_API void saveHDR(const Bitmap& bitmap, const char* filename)
	{
		stbi_write_hdr(filename, bitmap.w_, bitmap.h_, bitmap.comp_, reinterpret_cast<const float*>(bitmap.data_.data()));

	}

	IFNITY_API void printShaderSource(const char* text)
	{

		int line = 1;
		IFNITY_LOG(LogCore, TRACE, "Shader source code Start : -------------------------------------------------------------\n");
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
		IFNITY_LOG(LogCore, TRACE, "Shader source code END : -------------------------------------------------------------\n");

	}

	IFNITY_API std::string readShaderFile(const char* fileName)
	{
		FILE* file = fopen(fileName, "r");

		if(!file)
		{
			IFNITY_LOG(LogCore, ERROR, "I/O error. Cannot open shader file \n" + std::string(fileName));


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

	std::tuple<std::string, std::string, std::string> readShaderFilesByAPI(const rhi::GraphicsAPI& api, const IShader* vs, const IShader* fs, const IShader* gs)
	{
		if(!vs || !fs)
		{
			throw std::runtime_error("Vertex shader or fragment shader is missing.");
		}

		const auto& vsfile = vs->GetShaderDescpritionbyAPI(api).Filepath;
		const auto& psfile = fs->GetShaderDescpritionbyAPI(api).Filepath;
		const auto& gsfile = gs ? gs->GetShaderDescpritionbyAPI(api).Filepath : "";

		std::string vertexCode, fragmentCode, geometryCode;
		try
		{
			vertexCode = Utils::readShaderFile(vsfile.c_str());
			fragmentCode = Utils::readShaderFile(psfile.c_str());

			if(!gsfile.empty())
			{
				geometryCode = Utils::readShaderFile(gsfile.c_str());
			}
		}
		catch(const std::ifstream::failure& e)
		{
			throw std::runtime_error("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " + std::string(e.what()));
		}

		return { vertexCode, fragmentCode, geometryCode };
	}

	IFNITY_API uint32_t getTextureBytesPerLayer(uint32_t width, uint32_t height, rhi::Format format, uint32_t level) noexcept
	{
		const uint32_t levelWidth = std::max(width >> level, 1u);
		const uint32_t levelHeight = std::max(height >> level, 1u);
		
		const TextureFormatProperties props = properties[SCAST_U8(format)];

		if (!props.compressed) {
			return props.bytesPerBlock * levelWidth * levelHeight;
		}

		const uint32_t blockWidth = std::max((uint32_t)props.blockWidth, 1u);
		const uint32_t blockHeight = std::max((uint32_t)props.blockHeight, 1u);
		const uint32_t widthInBlocks = (levelWidth + props.blockWidth - 1) / props.blockWidth;
		const uint32_t heightInBlocks = (levelHeight + props.blockHeight - 1) / props.blockHeight;
		return widthInBlocks * heightInBlocks * props.bytesPerBlock;
	}

}

IFNITY_END_NAMESPACE