#pragma once 

#include "pch.h"
#include "Ifnity/Utils/VFS.hpp"
#include "Ifnity/Graphics/IShader.hpp"
#include <dxcapi.h>
#include <wrl\client.h>


//#pragma comment(lib, "dxcompiler.lib")

IFNITY_NAMESPACE


using Microsoft::WRL::ComPtr;

/**
 * @class ShaderCompiler
 * @brief Class for shader compilation.
 */
class IFNITY_API ShaderCompiler
{
public:
    /**
     * @brief Initializes the shader compiler.
     * @return true if initialization was successful, false otherwise.
     */
    static bool Initialize();

    /**
     * @brief Compiles a shader from source code.
     * @param shaderSource Shader source code.
     * @param entryPoint Shader entry point.
     * @param profile Shader profile (e.g., vs_5_0 for a vertex shader).
     * @param blob Pointer to the resulting compiled shader blob.
     * @param name Shader name.
     * @return HRESULT indicating the result of the compilation.
     */
    static HRESULT CompileShader(const std::wstring& shaderSource, const std::wstring& entryPoint, const std::wstring& profile, IDxcBlob** blob, std::string name);

    /**
     * @brief Compiles a shader from an IShader object.
     * @param shader Pointer to the IShader object.
     * @return HRESULT indicating the result of the compilation.
     */
    static HRESULT CompileShader(IShader* shader);

    /**
     * @brief Converts a SPIR-V file to GLSL.
     * @param inputFilePath Path to the input SPIR-V file.
     * @param outputFilePath Path to the output GLSL file.
     * @return true if the conversion was successful, false otherwise.
     */
    static bool ShaderCompiler::CompileSpirV2Glsl(const std::string& inputFilePath, const std::string& outputFilePath);

    /**
     * @brief Loads a SPIR-V file and returns its content as a vector of uint32_t.
     * @param filename Name of the SPIR-V file.
     * @return Vector of uint32_t containing the SPIR-V file content.
     */
    static std::vector<uint32_t> ShaderCompiler::load_spirv_file(const std::string& filename);

    /**
     * @brief Creates a shader blob from source code.
     * @param shaderSource Shader source code.
     * @param sourceBlob Resulting shader blob.
     * @return HRESULT indicating the result of the blob creation.
     */
    static HRESULT ShaderCompiler::CreateShaderBlob(const std::wstring& shaderSource, ComPtr<IDxcBlobEncoding>& sourceBlob);

    /**
     * @brief Compiles a shader blob.
     * @param sourceBlob Source code blob.
     * @param args Compilation arguments.
     * @param result Compilation result.
     * @return HRESULT indicating the result of the compilation.
     */
    static HRESULT ShaderCompiler::CompileShaderBlob(ComPtr<IDxcBlobEncoding>& sourceBlob, std::vector<const wchar_t*>& args, ComPtr<IDxcResult>& result);

    /**
	 * @brief Get Shader File Path in the VFS, this is used to get the shader file path storage in VFS.
     * @param vfs Virtual file system.
     * @param virtualPath Virtual path.
     * @param subdirectory Subdirectory.
     * @param fileName File name.
     * @param extension File extension.
     * @return Complete shader file path.
     */
    static std::string GetShaderFilePath(const VFS& vfs, const std::string& virtualPath, const std::string& subdirectory, const std::string& fileName, const std::string& extension);

private:
    /**
     * @brief Gets a reference to the virtual file system.
     * @return Reference to the virtual file system.
     */
    static VFS& GetVFS();

    /**
     * @brief Checks if the initialization was successful.
     * @return true if initialization was successful, false otherwise.
     */
    static bool CheckInitialization();

    /// Shader compiler.
    static ComPtr<IDxcCompiler3> m_compiler;
    /// DXC utilities.
    static ComPtr<IDxcUtils> m_utils;
};

IFNITY_END_NAMESPACE




