
#include "IMeshDataBuilder.hpp"
#include "Ifnity\Graphics\Interfaces\IMeshObject.hpp"
#include <Ifnity\Graphics\VtxData.hpp>




IFNITY_NAMESPACE

void MeshDataBuilderCacheFile::buildMeshData(MeshObjectDescription& description)
{
	IFNITY_LOG(LogCore, WARNING, "Not implemented yet");

}

void MeshDataBuilderAssimp::loadFileAssimp(const char* fileName, MeshData& meshData)
{

	printf("Loading '%s'...\n", fileName);

	const unsigned int flags = 0 |
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FindDegenerates |
		aiProcess_FindInvalidData |
		aiProcess_GenUVCoords;

	const aiScene* scene = aiImportFile(fileName, flags);

	if(!scene || !scene->HasMeshes())
	{
		printf("Unable to load '%s'\n", fileName);
		exit(255);
	}

	meshData.meshes_.reserve(scene->mNumMeshes);
	//meshData.boxes_.reserve(scene->mNumMeshes);

	for(unsigned int i = 0; i != scene->mNumMeshes; i++)
	{
		printf("\nConverting meshes %u/%u...", i + 1, scene->mNumMeshes);
		fflush(stdout);
		meshData.meshes_.push_back(convertAIMesh(scene->mMeshes[ i ],meshData));
	}













}

Mesh MeshDataBuilderAssimp::convertAIMesh(const aiMesh* m, MeshData& meshData)
{

	const bool hasTexCoords = m->HasTextureCoords(0);
	const uint32_t streamElementSize = static_cast<uint32_t>(m_numElementsPerVertex * sizeof(float));

	// Original data for LOD calculation
	std::vector<float> srcVertices;
	std::vector<uint32_t> srcIndices;

	std::vector<std::vector<uint32_t>> outLods;

	auto& vertices = meshData.vertexData_;

	for(size_t i = 0; i != m->mNumVertices; i++)
	{
		const aiVector3D v = m->mVertices[ i ];
		const aiVector3D n = m->mNormals[ i ];
		const aiVector3D t = hasTexCoords ? m->mTextureCoords[ 0 ][ i ] : aiVector3D();

		/*if(g_calculateLODs)
		{
			srcVertices.push_back(v.x);
			srcVertices.push_back(v.y);
			srcVertices.push_back(v.z);
		}*/

		vertices.push_back(v.x * m_meshScale);
		vertices.push_back(v.y * m_meshScale);
		vertices.push_back(v.z * m_meshScale);

		vertices.push_back(t.x);
		vertices.push_back(1.0f - t.y);

		vertices.push_back(n.x);
		vertices.push_back(n.y);
		vertices.push_back(n.z);
	}

	Mesh result =
	{
		.streamCount = 1,
		.indexOffset = m_indexOffset,
		.vertexOffset = m_vertexOffset,
		.vertexCount = m->mNumVertices,
		.streamOffset = { m_vertexOffset * streamElementSize },
		.streamElementSize = { streamElementSize }
	};

	for(size_t i = 0; i != m->mNumFaces; i++)
	{
		if(m->mFaces[ i ].mNumIndices != 3)
			continue;
		for(unsigned j = 0; j != m->mFaces[ i ].mNumIndices; j++)
			srcIndices.push_back(m->mFaces[ i ].mIndices[ j ]);
	}

	/*if(!g_calculateLODs)
		outLods.push_back(srcIndices);
	else
		processLods(srcIndices, srcVertices, outLods);*/

	printf("\nCalculated LOD count: %u\n", (unsigned)outLods.size());

	uint32_t numIndices = 0;
	for(size_t l = 0; l < outLods.size(); l++)
	{
		for(size_t i = 0; i < outLods[ l ].size(); i++)
			meshData.indexData_.push_back(outLods[ l ][ i ]);

		result.lodOffset[ l ] = numIndices;
		numIndices += (int)outLods[ l ].size();
	}

	result.lodOffset[ outLods.size() ] = numIndices;
	result.lodCount = (uint32_t)outLods.size();

	m_indexOffset += numIndices;
	m_vertexOffset += m->mNumVertices;

	return result;
}


void MeshDataBuilderAssimp::buildMeshData(MeshObjectDescription& description)
{
	IFNITY_LOG(LogCore, WARNING, "Not implemented yet");

}

void MeshDataBuilderGeometryModel::buildMeshData(MeshObjectDescription& description)
{
	using namespace GeometricModels;
	assert(!description.isLargeMesh, IFNITY_LOG(LogCore, ERROR, "error change use largeMesh to false , you want create geometric model ?"));

	switch(m_Type)
	{
	case GeometricalModelType::TETHAHEDRON:
		Tetrahedron().toMeshData(description.meshData);
		break;
	case GeometricalModelType::CUBE:
		Cube().toMeshData(description.meshData);
		break;
	default:
		break;
	}
}


IFNITY_END_NAMESPACE


