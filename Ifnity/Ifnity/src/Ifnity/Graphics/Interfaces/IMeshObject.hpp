#pragma once 


#include "pch.h"
#include "Ifnity/Graphics/VtxData.hpp"
#include "Ifnity/Models/GeometicModels.hpp"
#include "Ifnity/Graphics/Interfaces/IMeshDataBuilder.hpp"
#include "Ifnity/Scene/Scene.h"


IFNITY_NAMESPACE

using namespace GeometricModels;

//Forward declaration
struct DrawDescription;

struct IFNITY_API MeshObjectDescription
{
	

	// Constructor de movimient
	/*is based in filename */
	std::string  filePath;
	bool isLargeMesh = false;
	bool isGeometryModel = false;


	//MeshData 
	MeshData meshData;
	MeshFileHeader meshFileHeader;
	IMeshDataBuilder* meshDataBuilder = nullptr;
	SceneConfig sceneConfig;



	MeshObjectDescription& setFilePath(const std::string& value) { filePath = value; return *this; };
	constexpr MeshObjectDescription& setIsLargeMesh(bool value) { isLargeMesh = value; return *this; };
	constexpr MeshObjectDescription& setIsGeometryModel(bool value) { isGeometryModel = value; return *this; };
	MeshObjectDescription& setMeshData(const MeshData& value) { meshData = value; return *this; };

	

	//Destructor.
	~MeshObjectDescription()
	{
	}


};




class IFNITY_API IMeshObject
{
public:
	virtual void Draw() = 0;
	virtual void Draw(const DrawDescription& desc) = 0;
	virtual void DrawIndexed() = 0;

	virtual MeshObjectDescription& GetMeshObjectDescription() = 0;

};


using MeshObjectHandle = std::shared_ptr<IMeshObject>;

IFNITY_END_NAMESPACE