
#include "IMeshDataBuilder.hpp"
#include "Ifnity\Graphics\Interfaces\IMeshObject.hpp"
#include <Ifnity\Graphics\VtxData.hpp>
#include <Ifnity\Scene\Scene.h>
#include <Ifnity\Scene\Material.h>


#define _TEST_MODE

IFNITY_NAMESPACE

//Instance explicit Template 
template class IFNITY_API MeshDataBuilderAssimp<rhi::VertexScene>;
template class IFNITY_API MeshDataBuilderAssimp<rhi::VertexBasic>;


template <typename VertexType>
aiScene* MeshDataBuilderAssimp<VertexType>::getAIScenePointer(const char* fileName)
{
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
		IFNITY_LOG(LogCore, ERROR, "Unable to get aiScene  '%s'\n", fileName);
		return nullptr;
	}
	return const_cast<aiScene*>(scene);
}

template <typename VertexType>
void MeshDataBuilderAssimp<VertexType>::processScene(const SceneConfig& cfg, MeshData& meshData)
{
	IFNITY_LOG(LogCore, INFO, "Process Scene started");

	// Extract scale and information data
	m_meshScale = cfg.scale;

	// Extract base model path
	const std::size_t pathSeparator = cfg.fileName.find_last_of("/\\");
	const std::string basePath = (pathSeparator != std::string::npos) ? cfg.fileName.substr(0, pathSeparator + 1) : std::string();

	// First load the mesh data
	const aiScene* scene = getAIScenePointer(cfg.fileName.c_str());
	loadFileAssimp(scene, meshData);

	// Save the mesh data
	if(!cfg.outputMesh.empty())
	{
		saveMeshData(cfg.outputMesh.c_str(), meshData);
	}

	// Material Conversion
	Scene ourScene;
	std::vector<MaterialDescription> materials;
	std::vector<std::string>& materialNames = ourScene.materialNames_;
	std::vector<std::string> files;
	std::vector<std::string> opacityMaps;

	for(unsigned int m = 0; m < scene->mNumMaterials; m++)
	{
		aiMaterial* mm = scene->mMaterials[ m ];
		printf("Material [%s] %u\n", mm->GetName().C_Str(), m);
		materialNames.push_back(std::string(mm->GetName().C_Str()));
		MaterialDescription D = convertAIMaterialToDescription(mm, files, opacityMaps);
		materials.push_back(D);
		dumpMaterial(files, D);
	}

	// Texture processing, rescaling and packing
	convertAndDownscaleAllTextures(materials, basePath, files, opacityMaps);

	// Save materials
	saveMaterials(cfg.outputMaterials.c_str(), materials, files);

	std::vector<MaterialDescription> allMaterials;
	std::vector<std::string> files2;
	loadMaterials(cfg.outputMaterials.c_str(), allMaterials, files2);

	// Scene hierarchy conversion
	traverse(scene, ourScene, scene->mRootNode, -1, 0);
	saveScene(cfg.outputScene.c_str(), ourScene);

	// Check if scene is correct
	#ifdef _TEST_MODE
	Scene ourScene_check;
	loadScene(cfg.outputScene.c_str(), ourScene_check);
	if(!(ourScene_check == ourScene))
	{
		IFNITY_LOG(LogCore, ERROR, "The scene is not loading correctly TestMode %s", cfg.outputScene.c_str());
	}
	checkMaterialsAndFiles(materials, files, allMaterials, files2);
	#endif
}








void MeshDataBuilderCacheFile::buildMeshData(MeshObjectDescription& description)
{
	IFNITY_LOG(LogCore, WARNING, "Not implemented yet");

}

template <typename VertexType>
bool MeshDataBuilderAssimp<VertexType>::loadFileAssimp(const char* fileName, MeshData& meshData)
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
        return false;
    }

    meshData.meshes_.reserve(scene->mNumMeshes);

    for(unsigned int i = 0; i != scene->mNumMeshes; i++)
    {
        meshData.meshes_.push_back(convertAIMesh(scene->mMeshes[ i ], meshData));
    }

    return true;
}

template <typename VertexType>
bool MeshDataBuilderAssimp<VertexType>::loadFileAssimp(const aiScene* const scene, MeshData& meshData)
{
    if(!scene || !scene->HasMeshes())
    {
        IFNITY_LOG(LogCore, ERROR, "Unable to load aiScene");
        return false;
    }

    meshData.meshes_.reserve(scene->mNumMeshes);

    for(unsigned int i = 0; i != scene->mNumMeshes; i++)
    {
        meshData.meshes_.push_back(convertAIMesh(scene->mMeshes[ i ], meshData));
    }

    return true;
}

template <typename VertexType>
Mesh MeshDataBuilderAssimp<VertexType>::convertAIMesh(const aiMesh* m, MeshData& meshData)
{
    const bool hasTexCoords = m->HasTextureCoords(0);
    const uint32_t streamElementSize = static_cast<uint32_t>(m_numElementsPerVertex * sizeof(float));

    std::vector<float> srcVertices;
    std::vector<uint32_t> srcIndices;
    std::vector<std::vector<uint32_t>> outLods;

    auto& vertices = meshData.vertexData_;

    for(size_t i = 0; i != m->mNumVertices; i++)
    {
        const aiVector3D v = m->mVertices[ i ];
        const aiVector3D n = m->mNormals[ i ];
        const aiVector3D t = hasTexCoords ? m->mTextureCoords[ 0 ][ i ] : aiVector3D();

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

    outLods.push_back(srcIndices);

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

template <typename VertexType>
void MeshDataBuilderAssimp<VertexType>::buildMeshData(MeshObjectDescription& description)
{
    if(description.filePath.empty())
    {
        IFNITY_LOG(LogCore, ERROR, "error filePath is empty, you need set a file assimp file");
        return;
    }

    if(!loadFileAssimp(description.filePath.c_str(), description.meshData))
    {
        IFNITY_LOG(LogCore, ERROR, "error loading file %s", description.filePath.c_str());
    }

    IFNITY_LOG(LogCore, INFO, "Loaded file successfully", description.filePath.c_str());

    saveMeshData(description.filePath.c_str(), description.meshData, description.meshFileHeader);
}

template <typename VertexType>
void MeshDataBuilderAssimp<VertexType>::buildSceneData(MeshObjectDescription& description)
{
    processScene(description.sceneConfig, description.meshData);
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


