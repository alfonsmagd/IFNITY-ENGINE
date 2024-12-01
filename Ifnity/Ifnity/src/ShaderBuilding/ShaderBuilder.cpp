// Created: 23.02.2018 16:00


#include "ShaderBuilder.hpp"
#include <spirv_cross_c.h>
#include <iostream>
#include <fstream>
#include <cassert>


IFNITY_NAMESPACE

void error_callback(void* user_data, const char* diagnostic)
{
	IFNITY_LOG(LogCore, ERROR, diagnostic);
}

ComPtr<IDxcCompiler3> ShaderCompiler::m_compiler = nullptr;
ComPtr<IDxcUtils> ShaderCompiler::m_utils = nullptr;


VFS& ShaderCompiler::GetVFS()
{
	return VFS::GetInstance();
}

bool ShaderCompiler::CheckInitialization()
{
	if(!m_compiler)
	{
		IFNITY_LOG(LogApp, ERROR, "El compilador DXC no está inicializado.");
		return false;
	}

	if(!m_utils)
	{
		IFNITY_LOG(LogApp, ERROR, "Las utilidades DXC no están inicializadas.");
		return false;
	}

	return true; 
}
bool ShaderCompiler::Initialize()
{
	ComPtr<IDxcCompiler3> compiler;
	ComPtr<IDxcUtils> utils;

	if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))))
	{
		IFNITY_LOG(LogApp, ERROR, "Error al crear el compilador DXC.");
		return false;
	}

	if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))))
	{
		IFNITY_LOG(LogApp, ERROR, "Error al crear las utilidades DXC.");
		return false;
	}

	m_compiler = compiler;
	m_utils = utils;

	
}

HRESULT ShaderCompiler::CompileShader(const std::wstring& shaderSource, const std::wstring& entryPoint, const std::wstring& profile, IDxcBlob** blob,std::string name)
{
	if(!CheckInitialization())
	{
		return E_FAIL;
	}

	// Cargar el archivo de shader
	ComPtr<IDxcBlobEncoding> sourceBlob;
	/* HRESULT hr = m_utils->LoadFile(shaderSource.c_str(), nullptr, &sourceBlob);
	 if (FAILED(hr))
	 {
		 std::cerr << "Error al cargar el archivo de shader." << std::endl;
		 return hr;
	 }*/
	 // Crear un blob a partir de la cadena de shader
	HRESULT hr = m_utils->CreateBlobFromPinned(shaderSource.c_str(), static_cast<UINT32>(shaderSource.size() * sizeof(wchar_t)), 1200, &sourceBlob);
	if (FAILED(hr))
	{
		IFNITY_LOG(LogApp, ERROR, "Error al crear el blob del shader.");
		return hr;
	}

	// Crear argumentos de compilación
	const wchar_t* args[] = {
		L"-E", entryPoint.c_str(),
		L"-T", profile.c_str(),
		L"-Zpr",
		L"-spirv"
	};

	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
	sourceBuffer.Size = sourceBlob->GetBufferSize();
	sourceBuffer.Encoding = 1200; // ANSI code page

	// Compilar el shader
	ComPtr<IDxcResult> result;
	hr = m_compiler->Compile(
		&sourceBuffer,
		args,
		_countof(args),
		nullptr,
		IID_PPV_ARGS(&result)
	);

	// Verificar el estado de la compilación
	HRESULT status;
	hr = result->GetStatus(&status);
	if (FAILED(hr))
	{
		IFNITY_LOG(LogApp, ERROR, "Error al obtener el estado de la compilación.");
		return hr;
	}

	if (FAILED(status))
	{
		std::cerr << "La compilación del shader falló." << std::endl;
		// Puedes obtener más detalles del error aquí si es necesario
		return status;
	}

	// Obtener el resultado de la compilación en SPIR-V
	ComPtr<IDxcBlob> spirvBlob;
	hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&spirvBlob), nullptr);
	if (FAILED(hr))
	{
		std::cerr << "Error al obtener el blob SPIR-V." << std::endl;
		return hr;
	}

	// Guardar el resultado en vkShader 
	VFS& vfs = GetVFS();
	std::string path = vfs.ResolvePath("Shaders");

	vfs.SaveFile(path, name, 
		std::vector<char>(reinterpret_cast<char*>(spirvBlob->GetBufferPointer()),
			reinterpret_cast<char*>(spirvBlob->GetBufferPointer()) + spirvBlob->GetBufferSize()));

	std::ofstream outFile(name, std::ios::binary);
	if (!outFile)
	{
		std::cerr << "Error al abrir el archivo de salida." << std::endl;
		return E_FAIL;
	}

	outFile.write(reinterpret_cast<const char*>(spirvBlob->GetBufferPointer()), spirvBlob->GetBufferSize());
	outFile.close();


	return S_OK;
}



HRESULT ShaderCompiler::CreateShaderBlob(const std::wstring& shaderSource, ComPtr<IDxcBlobEncoding>& sourceBlob)
{
	HRESULT hr = m_utils->CreateBlobFromPinned(shaderSource.c_str(), static_cast<UINT32>(shaderSource.size() * sizeof(wchar_t)), 1200, &sourceBlob);
	if(FAILED(hr))
	{
		IFNITY_LOG(LogApp, ERROR, "Error al crear el blob del shader.");
	}
	return hr;
}


HRESULT ShaderCompiler::CompileShaderBlob(ComPtr<IDxcBlobEncoding>& sourceBlob,  std::vector<const wchar_t*>& args, ComPtr<IDxcResult>& result)
{
	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
	sourceBuffer.Size = sourceBlob->GetBufferSize();
	sourceBuffer.Encoding = 1200; // ANSI code page

	HRESULT hr = m_compiler->Compile(
		&sourceBuffer,
		args.data(),
		static_cast<uint32_t>(args.size()),
		nullptr,
		IID_PPV_ARGS(&result)
	);

	if(FAILED(hr))
	{
		IFNITY_LOG(LogApp, ERROR, "Error al compilar el shader.");
		return hr;
	}

	HRESULT status;
	hr = result->GetStatus(&status);
	if(FAILED(hr))
	{
		IFNITY_LOG(LogApp, ERROR, "Error al obtener el estado de la compilación.");
		return hr;
	}

	if(FAILED(status))
	{
		std::cerr << "La compilación del shader falló." << std::endl;
		return status;
	}

	return S_OK;
}


std::string ShaderCompiler::GetShaderFilePath(const VFS& vfs, const std::string& virtualPath, const std::string& subdirectory, const std::string& fileName, const std::string& extension)
{
	return vfs.ResolvePath(virtualPath, subdirectory) + "/" + fileName + "." + extension;	
}


HRESULT ShaderCompiler::ChekingLocalShaders(IShader& shader)
{
	//Get the shader description
	ShaderCreateDescription description = shader.GetShaderDescription();
	//Get the VFS
	VFS& vfs = GetVFS();
	//Get the path to the shader
	std::string path = vfs.ResolvePath("Shaders","d3d12");
	//First d3d12 shader
	description.Filepath = path + "//" + description.FileName + ".hlsl";

	shader.AddShaderDescription(rhi::GraphicsAPI::D3D12, description);

	//Reslolve spirv path
	path = vfs.ResolvePath("Shaders", "vk");
	description.Filepath = path + "//" + description.FileName + ".spv";
	shader.AddShaderDescription(rhi::GraphicsAPI::VULKAN, description);

	//Resolve glsl path
	path = vfs.ResolvePath("Shaders", "opengl");
	description.Filepath = path + "//" + description.FileName + ".glsl";
	shader.AddShaderDescription(rhi::GraphicsAPI::OPENGL, description);

	return S_OK;
	
}


HRESULT ShaderCompiler::CompileShader(IShader* shader)
{
	// Obtener la descripción del shader
	ShaderCreateDescription description = shader->GetShaderDescription();

	//Check if not force to compile process 

	if(description.NoCompile)
	{
		return ChekingLocalShaders(*shader);
	}


	if(!CheckInitialization() || shader == nullptr)
	{
		return E_FAIL;
	}

	
	shader->AddShaderDescription(rhi::GraphicsAPI::D3D12, description);

	// Crear el blob del shader
	ComPtr<IDxcBlobEncoding> sourceBlob;
	HRESULT hr = CreateShaderBlob(description.ShaderSource, sourceBlob);
	if(FAILED(hr))
	{
		return hr;
	}

	// Compilar el shader
	ComPtr<IDxcResult> result;
	std::vector<const wchar_t*> compileArgs = shader->GetCompileArgs(description);
	hr = CompileShaderBlob(sourceBlob, compileArgs, result);
	if(FAILED(hr))
	{
		return hr;
	}

	// Obtener el resultado de la compilación en SPIR-V
	ComPtr<IDxcBlob> spirvBlob;
	hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&spirvBlob), nullptr);
	if(FAILED(hr))
	{
		std::cerr << "Error al obtener el blob SPIR-V." << std::endl;
		return hr;
	}

	// Guardar el shader en SPIR-V
	VFS& vfs = GetVFS();
	std::string path = vfs.ResolvePath("Shaders");	
	auto filepath = vfs.SaveFile(path, description.FileName + ".spv",
								std::vector<char>(reinterpret_cast<char*>(spirvBlob->GetBufferPointer()),
												  reinterpret_cast<char*>(spirvBlob->GetBufferPointer()) + spirvBlob->GetBufferSize()));

	description.Filepath = filepath;
	shader->AddShaderDescription(rhi::GraphicsAPI::VULKAN, description);

	// Compilar SPIR-V a GLSL
	if(!ShaderCompiler::CompileSpirV2Glsl(description.FileName + ".spv", description.FileName + ".glsl"))
	{
		return E_FAIL;
	}

	// Guardar el shader en GLSL
	description.Filepath = ShaderCompiler::GetShaderFilePath(vfs, "Shaders", "opengl", description.FileName, "glsl");
	shader->AddShaderDescription(rhi::GraphicsAPI::OPENGL, description);

	return S_OK;
	
}


std::vector<uint32_t>  ShaderCompiler::load_spirv_file(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		throw std::runtime_error("No se pudo abrir el archivo SPIR-V.");
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size % 4 != 0)
	{
		throw std::runtime_error("El tamaño del archivo SPIR-V no es múltiplo de 4.");
	}

	std::vector<uint32_t> buffer(size / 4);
	if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
	{
		throw std::runtime_error("Error al leer el archivo SPIR-V.");
	}

	return buffer;
}

bool ShaderCompiler::CompileSpirV2Glsl(const std::string& inputFilePath, const std::string& outputFilePath)
{

	//GetPath to get the file in vk --> spriv,spv. 
	VFS& vfs = GetVFS();
	std::string path = vfs.ResolvePath("Shaders","vk");

	// Complete 


	std::vector<uint32_t> spirv_binary = load_spirv_file(path + "//" +inputFilePath);

	spvc_context context = NULL;
	spvc_parsed_ir ir = NULL;
	spvc_compiler compiler_glsl = NULL;
	spvc_compiler_options options = NULL;
	spvc_resources resources = NULL;
	const spvc_reflected_resource* list = NULL;
	const char* result = NULL;
	size_t count;
	size_t i;

	// Crear contexto
	spvc_context_create(&context);

	// Establecer callback de error
	spvc_context_set_error_callback(context, error_callback, nullptr);

	// Parsear el SPIR-V
	spvc_context_parse_spirv(context, spirv_binary.data(), spirv_binary.size(), &ir);

	// Crear una instancia del compilador y darle propiedad del IR
	spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler_glsl);

	// Realizar reflexión básica
	spvc_compiler_create_shader_resources(compiler_glsl, &resources);
	spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &list, &count);

	for (i = 0; i < count; i++)
	{
		printf("ID: %u, BaseTypeID: %u, TypeID: %u, Name: %s\n", list[i].id, list[i].base_type_id, list[i].type_id, list[i].name);
		printf("  Set: %u, Binding: %u\n", spvc_compiler_get_decoration(compiler_glsl, list[i].id, SpvDecorationDescriptorSet), spvc_compiler_get_decoration(compiler_glsl, list[i].id, SpvDecorationBinding));
	}

	// Modificar opciones
	spvc_compiler_create_compiler_options(compiler_glsl, &options);
	spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_GLSL_SEPARATE_SHADER_OBJECTS, 450);
	spvc_compiler_install_compiler_options(compiler_glsl, options);
	// Construir combinaciones de imagen y samplers
	spvc_compiler_build_combined_image_samplers(compiler_glsl); // not delete its is importan to build 
	// Compilar a GLSL
	spvc_compiler_compile(compiler_glsl, &result);
	IFNITY_LOG(LogApp,INFO,("Cross-compiled source: %s\n", result));


	// Guardar el resultado en vkShader 
	
	 path = vfs.ResolvePath("Shaders");

	vfs.SaveFile(path, outputFilePath,
		std::vector<char>(result,
			result + strlen(result)));


	// Guardar la salida en un fichero
	std::ofstream outFile(outputFilePath);
	if (outFile.is_open())
	{
		outFile << result;
		outFile.close();
	}
	else
	{
		IFNITY_LOG(LogApp, ERROR, "We cant create the file.");
		spvc_context_destroy(context);
		return false;
	}

	// Liberar toda la memoria asignada hasta ahora
	spvc_context_destroy(context);
	return true;
}

IFNITY_END_NAMESPACE