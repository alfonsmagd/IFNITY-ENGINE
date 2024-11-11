
#pragma once	


#include "pch.h"
#include <set>

IFNITY_NAMESPACE


enum IFNITY_API ShaderType: uint8_t
{
	VERTEX_SHADER = 0x00,
	PIXEL_SHADER = 0x01,
	GEOMETRY_SHADER = 0x02,
	COMPUTE_SHADER = 0x03,
	HULL_SHADER = 0x04,
	DOMAIN_SHADER = 0x05,
	UNKNOWN_SHADER = 0x06
};


enum IFNITY_API ShaderCompileFlagType: uint32_t
{
	OPTIMIZE_SIZE = 0x1,
	ENABLE_DEBUG_INFO = 0x2,
	ENABLE_SPIRV = 0x4,
	PACK_MATRIX_ROW_MAJOR = 0x8,

	DEFAULT_FLAG = 0x0C
	// Agrega m�s flags seg�n sea necesario
};



struct IFNITY_API ShaderCreateDescription
{
	ShaderType Type;
	std::wstring ShaderSource;
	std::wstring EntryPoint;
	std::wstring Profile;
	ShaderCompileFlagType Flags;
	bool SaveFile{ true };
	std::string FileName;
	std::string Filepath{};
	bool NoCompile = false;

	// Operator to compare two ShaderCreateDescription and uses in std::set
	bool operator<(const ShaderCreateDescription& other) const
	{
		return std::tie(FileName, Type) != std::tie(other.FileName, other.Type);
	}
};


/// <summary>
/// Shader Interface to be implemented by the graphics API Inside 
/// </summary>
class IFNITY_API IShader
{
public:
	//Constructor 
	IShader() = default;
	//Destructor
	virtual ~IShader() = default;

	void SetShaderDescription(const ShaderCreateDescription& description) { m_Description = description; }
	
	ShaderCreateDescription GetShaderDescription() const { return m_Description; }
	const ShaderCreateDescription& GetShaderDescpritionbyAPI(rhi::GraphicsAPI api) const ;
	
	void AddShaderDescription(rhi::GraphicsAPI api, const ShaderCreateDescription& description);
	std::vector<const wchar_t*> GetCompileArgs(const ShaderCreateDescription& shaderDescription) const;
	

private:
	std::vector<const wchar_t*> CreateCompileArgs(const ShaderCreateDescription& shaderDescription) const;

	std::map<rhi::GraphicsAPI, ShaderCreateDescription> shaderMap;

	ShaderCreateDescription m_Description;

};

























IFNITY_END_NAMESPACE