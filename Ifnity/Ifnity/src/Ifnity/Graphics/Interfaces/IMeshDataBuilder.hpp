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
};

class MeshDataBuilderAssimp: public IMeshDataBuilder
{
private:
	 void loadFileAssimp(const char* fileName, MeshData& meshData);
	 Mesh convertAIMesh(const aiMesh* m,       MeshData& meshData);
	
public:
	MeshDataBuilderAssimp(uint16_t numElementsPerVertex, float meshScale): m_numElementsPerVertex(numElementsPerVertex), m_meshScale(meshScale) {};

	void buildMeshData(MeshObjectDescription& description) override;

private:
	const uint16_t m_numElementsPerVertex;
	const float    m_meshScale;
	uint32_t       m_indexOffset = 0;
	uint32_t       m_vertexOffset = 0;

};

class MeshDataBuilderCacheFile: public IMeshDataBuilder
{
public:
	void buildMeshData(MeshObjectDescription& description) override;

};

class MeshDataBuilderGeometryModel: public IMeshDataBuilder
{
public:
	MeshDataBuilderGeometryModel(GeometricModels::GeometricalModelType type): m_Type(type) {};

	void buildMeshData(MeshObjectDescription& description) override;

private:
	GeometricModels::GeometricalModelType m_Type;
};

IFNITY_END_NAMESPACE