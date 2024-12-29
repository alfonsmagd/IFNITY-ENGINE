#pragma once
#include "pch.h"
#include "Ifnity/Models/GeometicModels.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

IFNITY_NAMESPACE



struct  MeshObjectDescription;

class IFNITY_API IMeshDataBuilder
{
public:
	virtual ~IMeshDataBuilder() = default;

	virtual void buildMeshData(MeshObjectDescription& description) = 0;
	inline virtual void buildSceneData(MeshObjectDescription& description) {};
};

class  IFNITY_API MeshDataBuilderAssimp: public IMeshDataBuilder
{

public:
	MeshDataBuilderAssimp(uint16_t numElementsPerVertex, float meshScale): m_numElementsPerVertex(numElementsPerVertex), m_meshScale(meshScale) {};

	MeshDataBuilderAssimp(uint16_t numElemntsPerVertex): m_numElementsPerVertex(numElemntsPerVertex) {};
	
	void buildMeshData(MeshObjectDescription& description) override;
	void buildSceneData(MeshObjectDescription& description) override;

private:
	bool loadFileAssimp(const char* fileName, MeshData& meshData);
	bool loadFileAssimp(const aiScene* const scene, MeshData& meshData);
	[[nodiscard]] aiScene* getAIScenePointer(const char* fileName);
	Mesh convertAIMesh(const aiMesh* m, MeshData& meshData);
	void processScene(const struct SceneConfig& cfg, MeshData& meshData);
private:
	const uint16_t m_numElementsPerVertex;  // Number of elements per vertex pos,normal,tg, example
	float		   m_meshScale;
	uint32_t       m_indexOffset = 0;
	uint32_t       m_vertexOffset = 0;
	

};

class IFNITY_API MeshDataBuilderCacheFile: public IMeshDataBuilder
{
public:
	void buildMeshData(MeshObjectDescription& description) override;

};

class IFNITY_API MeshDataBuilderGeometryModel: public IMeshDataBuilder
{
public:
	MeshDataBuilderGeometryModel(GeometricModels::GeometricalModelType type): m_Type(type) {};

	void buildMeshData(MeshObjectDescription& description) override;

private:
	GeometricModels::GeometricalModelType m_Type;
};

IFNITY_END_NAMESPACE