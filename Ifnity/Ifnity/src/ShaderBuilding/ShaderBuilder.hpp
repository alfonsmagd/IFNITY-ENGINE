#pragma once 

#include "pch.h"
#include "Ifnity/Utils/VFS.hpp"
#include "Ifnity/Graphics/IShader.hpp"
#include <dxc/dxcapi.h>
#include <wrl\client.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>


#pragma comment(lib, "dxcompiler.lib")
IFNITY_NAMESPACE



using  Microsoft::WRL::ComPtr;

class IFNITY_API ShaderCompiler
{
public:
    static bool Initialize();
    
    static HRESULT CompileShader(const std::wstring& shaderSource, const std::wstring& entryPoint, const std::wstring& profile, IDxcBlob** blob, std::string name);

	static HRESULT CompileShader(IShader* shader);

    static bool ShaderCompiler::CompileSpirV2Glsl(const std::string& inputFilePath, const std::string& outputFilePath);

    static std::vector<uint32_t>  ShaderCompiler::load_spirv_file(const std::string& filename);


private:
    static VFS& GetVFS();

	static bool CheckInitialization();


    static ComPtr<IDxcCompiler3> m_compiler;
    static ComPtr<IDxcUtils> m_utils;
};


IFNITY_END_NAMESPACE