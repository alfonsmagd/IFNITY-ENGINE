

#include "IShader.hpp"








IFNITY_NAMESPACE

std::vector<const wchar_t*> IShader::CreateCompileArgs( const ShaderCreateDescription& shaderDescription ) const
{
	//Reserve only the space for the arguments that are going to be used
	std::vector<const wchar_t*> args;
	args.reserve( 10 );

	// Argumentos obligatorios
	args = { L"-E", shaderDescription.EntryPoint.c_str(), L"-T", shaderDescription.Profile.c_str() };

	ShaderCompileFlagType flags = shaderDescription.Flags;

	// Flags. 
	if( flags & OPTIMIZE_SIZE )     args.push_back( L"-O1" );
	if( flags & ENABLE_DEBUG_INFO ) args.push_back( L"-Zi" );
	if( flags & ENABLE_SPIRV )      args.push_back( L"-spirv" );
	if( flags & PACK_MATRIX_ROW_MAJOR ) args.push_back( L"-Zpr" );
	else
	{
		//Column Major
		args.push_back( L"-Zpc" );
	
	}
	args.push_back( L"-HV" ); // HLSL version
	args.push_back( L"2021" ); // HLSL version 2021
	return args;
}

std::vector<const wchar_t*> IShader::GetCompileArgs( const ShaderCreateDescription& shaderDescription ) const
{
	return CreateCompileArgs( shaderDescription );

}

void IShader::AddShaderDescription( rhi::GraphicsAPI api, const ShaderCreateDescription& description )
{
	shaderMap[ api ] = (description);
}



const ShaderCreateDescription& IShader::GetShaderDescpritionbyAPI( rhi::GraphicsAPI api ) const
{
	auto it = shaderMap.find( api );
	if( it != shaderMap.end() )
	{
		return it->second;
	}
	// Manejar el caso en que no se encuentra el API
	return ShaderCreateDescription{};
}


IFNITY_END_NAMESPACE

