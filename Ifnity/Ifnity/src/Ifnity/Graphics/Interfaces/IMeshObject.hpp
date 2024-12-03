#pragma once 


#include "pch.h"
#include "Ifnity/Graphics/VtxData.hpp"
#include "Ifnity/Models/GeometicModels.hpp"

IFNITY_NAMESPACE

using namespace GeometricModels;

struct IFNITY_API MeshObjectDescription
{


	/*is based in filename */
	std::string  filePath;
	bool isLargeMesh = false;
	bool isGeometryModel = false;

	//MeshData 
	MeshData meshData;
	MeshFileHeader meshFileHeader;
	

	MeshObjectDescription&           setFilePath(const std::string& value) { filePath = value; return *this; };
	constexpr MeshObjectDescription& setIsLargeMesh(bool value) { isLargeMesh = value; return *this; };
	constexpr MeshObjectDescription& setIsGeometryModel(bool value) { isGeometryModel = value; return *this; };
	 MeshObjectDescription& setMeshData(const MeshData& value) { meshData = value; return *this; };
	MeshObjectDescription& buildMeshDatabyAssimp()
	{
		//Todo this process 
		return *this;

	}

	MeshObjectDescription& buildMeshDataByCacheFile()
	{
		//Todo this process 
		return *this;
	}

	MeshObjectDescription& buildMeshDataByGeometryModel(GeometricalModelType type)
	{
	
		assert(!isLargeMesh, "Not posible to build , you set isLargeMesh true , but you choose buildMeshData");
		
		//Build switch case to build the mesh data by geometry model
		//Create switch 
		switch(type)
		{
		case GeometricalModelType::TETHAHEDRON:
		{
			Tetrahedron().toMeshData(meshData);
			break;
		}
		case GeometricalModelType::CUBE:
		{
			Cube().toMeshData(meshData);
			break;
		}
		default:
			break;
		}

		return *this;
	}


};


class IFNITY_API IMeshObject
{
public:
	virtual void Draw() = 0;

};


using MeshObjectHandle = std::shared_ptr<IMeshObject>;

IFNITY_END_NAMESPACE