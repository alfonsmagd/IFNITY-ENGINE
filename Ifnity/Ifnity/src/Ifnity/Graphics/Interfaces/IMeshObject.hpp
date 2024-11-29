#pragma once 


#include "pch.h"
#include "Ifnity\Models\GeometicModels.hpp"

IFNITY_NAMESPACE



struct MeshDescription
{
	/*is based in filename */
	std::string  filePath;
	bool isLargeMesh = false;
	bool isGeometryModel = false;
	

	 MeshDescription& setFilePath(const std::string& value) { filePath = value; return *this; };
	constexpr MeshDescription& setIsLargeMesh(bool value) { isLargeMesh = value; return *this; };
	constexpr MeshDescription& setIsGeometryModel(bool value) { isGeometryModel = value; return *this; };


};


class IMeshObject
{


	virtual void Initialize() = 0;
	virtual void Draw() = 0;
	virtual void Destroy() = 0;

};


using MeshObjectHandle = std::shared_ptr<IMeshObject>;

IFNITY_END_NAMESPACE