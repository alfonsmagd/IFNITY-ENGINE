#include "VtxData.hpp"
#include "Ifnity\Utils\VFS.hpp"

IFNITY_NAMESPACE


IFNITY_API MeshFileHeader loadMeshData(const char* meshFile, MeshData& out)
{

	FILE* f = fopen(meshFile, "rb");

	assert(f); 

	if(!f)
	{
		IFNITY_LOG(LogApp, ERROR, "Cannot open %s. Did you forget to run \"Convert mesh \"?\n", meshFile);
		exit(EXIT_FAILURE);
	}

	MeshFileHeader fileHeader;

	// Read the header and check if sizeof its correct 
	if(fread(&fileHeader, 1, sizeof(fileHeader), f) != sizeof(fileHeader))
	{
		IFNITY_LOG(LogApp, ERROR, "Unable to read mesh file fileHeader\n");
		exit(EXIT_FAILURE);
	}

	// Copy meshfileheader 
	auto header = fileHeader;


	// Get mesh count  and fill the mesh data
	out.meshes_.resize(fileHeader.meshCount);
	if(fread(out.meshes_.data(), sizeof(Mesh), fileHeader.meshCount, f) != fileHeader.meshCount)
	{
		IFNITY_LOG(LogApp, ERROR, "Could not read mesh descriptors\n");
		exit(EXIT_FAILURE);
	}


	out.indexData_.resize(fileHeader.indexDataSize / sizeof(uint32_t));
	out.vertexData_.resize(fileHeader.vertexDataSize / sizeof(float));

	if((fread(out.indexData_.data(), 1, fileHeader.indexDataSize, f) != fileHeader.indexDataSize) ||
		(fread(out.vertexData_.data(), 1, fileHeader.vertexDataSize, f) != fileHeader.vertexDataSize))
	{
		IFNITY_LOG(LogApp, ERROR, "Unable to read index/vertex data\n");
		exit(255);
	}

	fclose(f);

	return fileHeader;
}


//TODO TEMPLATE..
IFNITY_API void combineBuffers(const std::vector<uint32_t>& indexData,
	const std::vector<float>& vertexData, std::vector<uint8_t>& combinedBuffer)
{
	// Calcular el tamaño combinado
	size_t totalSize = indexData.size() * sizeof(uint32_t) + vertexData.size() * sizeof(float);
	combinedBuffer.resize(totalSize);

	// Copiar índices al inicio del buffer combinado
	std::memcpy(combinedBuffer.data(), indexData.data(), indexData.size() * sizeof(uint32_t));

	// Copiar vértices después de los índices
	std::memcpy(combinedBuffer.data() + indexData.size() * sizeof(uint32_t),
		vertexData.data(),
		vertexData.size() * sizeof(float));
}
IFNITY_API void loadCombinedBuffer(FILE* file, MeshFileHeader& header, std::vector<uint8_t>& combinedBuffer)
{
	// Calcular tamaño combinado
	size_t totalSize = header.indexDataSize * sizeof(uint32_t) + header.vertexDataSize * sizeof(float);

	// Reservar espacio en el buffer combinado
	combinedBuffer.resize(totalSize);

	// Leer datos al buffer combinado directamente
	fread(combinedBuffer.data(), 1, totalSize, file);
}




IFNITY_API void saveMeshData(const char* meshFile, const MeshData& data,  MeshFileHeader& fheader)
{
	//Generate  the file with .meshdata extension 
	std::string meshFileStr = meshFile + std::string(".meshdata");

	FILE* f = fopen(meshFileStr.c_str(), "wb");

	assert(f);

	if(!f)
	{
		IFNITY_LOG(LogApp, ERROR, "Cannot open %s", meshFile);
		exit(EXIT_FAILURE);
	}

	const MeshFileHeader header{
		.magicValue = 0x12345678,
		.meshCount = static_cast<uint32_t>(data.meshes_.size()),
		.dataBlockStartOffset = (uint32_t)(sizeof(MeshFileHeader) + data.meshes_.size() * sizeof(Mesh)),
		.indexDataSize = static_cast<uint32_t>(data.indexData_.size() * sizeof(uint32_t)),
		.vertexDataSize = static_cast<uint32_t>(data.vertexData_.size() * sizeof(float))
	};


	fheader.magicValue = header.magicValue;
	fheader.meshCount = header.meshCount;
	fheader.dataBlockStartOffset = header.dataBlockStartOffset;
	fheader.indexDataSize = header.indexDataSize;
	fheader.vertexDataSize = header.vertexDataSize;


	// Write the header
	fwrite(&header, 1, sizeof(header), f);

	// Write the mesh descriptors
	fwrite(data.meshes_.data(), sizeof(Mesh), data.meshes_.size(), f);

	// Write the index and vertex data
	fwrite(data.indexData_.data(), 1, header.indexDataSize, f);
	fwrite(data.vertexData_.data(), 1, header.vertexDataSize, f);

	fclose(f);


}

IFNITY_API void saveMeshData(const char* meshFile, const MeshData& data)
{
	// Generar el archivo con la extensión .meshdata
	std::string meshFileStr = meshFile;

	FILE* f = fopen(meshFileStr.c_str(), "wb");


	if(!f)
	{
		IFNITY_LOG(LogApp, ERROR, "Cannot open %s", meshFile);
		exit(EXIT_FAILURE);
	}

	const MeshFileHeader header{
		.magicValue = 0x12345678,
		.meshCount = static_cast<uint32_t>(data.meshes_.size()),
		.dataBlockStartOffset = (uint32_t)(sizeof(MeshFileHeader) + data.meshes_.size() * sizeof(Mesh)),
		.indexDataSize = static_cast<uint32_t>(data.indexData_.size() * sizeof(uint32_t)),
		.vertexDataSize = static_cast<uint32_t>(data.vertexData_.size() * sizeof(float))
	};

	// Escribir el encabezado
	fwrite(&header, 1, sizeof(header), f);

	// Escribir los descriptores de malla
	fwrite(data.meshes_.data(), sizeof(Mesh), data.meshes_.size(), f);

	// Escribir los datos de índice y vértice
	fwrite(data.indexData_.data(), 1, header.indexDataSize, f);
	fwrite(data.vertexData_.data(), 1, header.vertexDataSize, f);

	fclose(f);
}

IFNITY_END_NAMESPACE


