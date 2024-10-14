#pragma once 

#include "pch.h"

#include <dxc/dxcapi.h>
#include <wrl\client.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>


#pragma comment(lib, "dxcompiler.lib")
IFNITY_NAMESPACE



using  Microsoft::WRL::ComPtr;

class  ShaderCompiler
{
public:
    static bool Initialize();
    
    static HRESULT CompileShader(const std::wstring& shaderSource, const std::wstring& entryPoint, const std::wstring& profile, IDxcBlob** blob, std::string name);

    static bool ShaderCompiler::CompileSpirV2Glsl(const std::string& inputFilePath, const std::string& outputFilePath);

    static std::vector<uint32_t>  ShaderCompiler::load_spirv_file(const std::string& filename);

private:
    static ComPtr<IDxcCompiler3> m_compiler;
    static ComPtr<IDxcUtils> m_utils;
};


IFNITY_END_NAMESPACE