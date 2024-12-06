#pragma once
#include "pch.h"
#include "Ifnity/Models/GeometicModels.hpp"


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
public:
	void buildMeshData(MeshObjectDescription& description) override;

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