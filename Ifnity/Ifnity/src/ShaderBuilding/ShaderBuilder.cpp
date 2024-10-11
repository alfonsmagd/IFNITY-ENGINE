// Created: 23.02.2018 16:00


#include "ShaderBuilder.hpp"

#include <iostream>
#include <fstream>


IFNITY_NAMESPACE

ComPtr<IDxcCompiler3> ShaderCompiler::m_compiler = nullptr;
ComPtr<IDxcUtils> ShaderCompiler::m_utils = nullptr;


bool ShaderCompiler::Initialize()
{
	ComPtr<IDxcCompiler3> compiler;
	ComPtr<IDxcUtils> utils;

	if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))))
	{
		std::cerr << "Error al crear el compilador DXC." << std::endl;
		return false;
	}

	if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))))
	{
		std::cerr << "Error al crear las utilidades DXC." << std::endl;
		return false;
	}

	m_compiler = compiler;
	m_utils = utils;

	
}

HRESULT ShaderCompiler::CompileShader(const std::wstring& shaderSource, const std::wstring& entryPoint, const std::wstring& profile, IDxcBlob** blob,std::string name)
{
	if (!m_compiler)
	{
		std::cerr << "El compilador DXC no está inicializado." << std::endl;
		return E_FAIL;
	}

	if (!m_utils)
	{
		std::cerr << "Las utilidades DXC no están inicializadas." << std::endl;
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
		std::cerr << "Error al crear el blob del shader." << std::endl;
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
		std::cerr << "Error al obtener el estado de la compilación." << std::endl;
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

	// Guardar el resultado en un archivo
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



IFNITY_END_NAMESPACE