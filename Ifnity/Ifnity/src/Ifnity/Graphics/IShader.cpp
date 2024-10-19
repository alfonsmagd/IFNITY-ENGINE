

#include "IShader.hpp"








IFNITY_NAMESPACE

std::vector<const wchar_t*> IShader::CreateCompileArgs(const ShaderCreateDescription& shaderDescription) const
{
	//Reserve only the space for the arguments that are going to be used
    std::vector<const wchar_t*> args;
    args.reserve(10); 

    // Argumentos obligatorios
    args = { L"-E", shaderDescription.EntryPoint.c_str(), L"-T", shaderDescription.Profile.c_str() };

	ShaderCompileFlagType flags = shaderDescription.Flags;

    // Flags. 
    if(flags & OPTIMIZE_SIZE)     args.push_back(L"-Osize");
    if(flags & ENABLE_DEBUG_INFO) args.push_back(L"-Zi");
    if(flags & ENABLE_SPIRV)      args.push_back(L"-spirv");
    if(flags & PACK_MATRIX_ROW_MAJOR) args.push_back(L"-Zpr");

    return args;
}

std::vector<const wchar_t*> IShader::GetCompileArgs(const ShaderCreateDescription& shaderDescription) const
{
    return CreateCompileArgs(shaderDescription);

}

void IShader::AddShaderDescription(rhi::GraphicsAPI api, const ShaderCreateDescription& description)
{
    shaderMap[ api ] = (description);
}



IFNITY_END_NAMESPACE

