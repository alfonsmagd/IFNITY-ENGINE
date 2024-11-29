#include "VtxData.hpp"

IFNITY_NAMESPACE


MeshFileHeader loadMeshData(const char* meshFile, MeshData& out)
{
	MeshFileHeader header;

	FILE* f = fopen(meshFile, "rb");

	assert(f); // Did you forget to run "Ch5_Tool05_MeshConvert"?

	if(!f)
	{
		IFNITY_LOG(LogApp,ERROR,"Cannot open %s. Did you forget to run \"Convert mesh \"?\n", meshFile);
		exit(EXIT_FAILURE);
	}

	// Read the header and check if sizeof its correct 
	if(fread(&header, 1, sizeof(header), f) != sizeof(header))
	{
		IFNITY_LOG(LogApp,ERROR,"Unable to read mesh file header\n");
		exit(EXIT_FAILURE);
	}

	// Get mesh count  and fill the mesh data
	out.meshes_.resize(header.meshCount);
	if(fread(out.meshes_.data(), sizeof(Mesh), header.meshCount, f) != header.meshCount)
	{
		IFNITY_LOG(LogApp,ERROR,"Could not read mesh descriptors\n");
		exit(EXIT_FAILURE);
	}
	

	out.indexData_.resize(header.indexDataSize / sizeof(uint32_t));
	out.vertexData_.resize(header.vertexDataSize / sizeof(float));

	if((fread(out.indexData_.data(), 1, header.indexDataSize, f) != header.indexDataSize) ||
		(fread(out.vertexData_.data(), 1, header.vertexDataSize, f) != header.vertexDataSize))
	{
		IFNITY_LOG(LogApp, ERROR, "Unable to read index/vertex data\n");
		exit(255);
	}

	fclose(f);

	return header;
}


//TODO TEMPLATE..
void combineBuffers(const std::vector<uint32_t>& indexData,
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
void loadCombinedBuffer(FILE* file, MeshFileHeader& header, std::vector<uint8_t>& combinedBuffer)
{
	// Calcular tamaño combinado
	size_t totalSize = header.indexDataSize * sizeof(uint32_t) + header.vertexDataSize * sizeof(float);

	// Reservar espacio en el buffer combinado
	combinedBuffer.resize(totalSize);

	// Leer datos al buffer combinado directamente
	fread(combinedBuffer.data(), 1, totalSize, file);
}



// 






IFNITY_END_NAMESPACE


