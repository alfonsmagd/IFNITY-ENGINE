#pragma once 


#include "pch.h"
#include "../../Graphics/VtxData.hpp"

IFNITY_NAMESPACE



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

	MeshObjectDescription& buildMeshDataByGeometryModel()
	{
		IFNITY_LOG(LogApp, ERROR, "Not posible to build , you not uses a GeometricalBaseModel to build");
		assert(!isGeometryModel, "Not posible to build , you not uses a GeometricalBaseModel to build");
		//Todo this process 
		return *this;
	}


};


class IFNITY_API IMeshObject
{



};


using MeshObjectHandle = std::shared_ptr<IMeshObject>;

IFNITY_END_NAMESPACE