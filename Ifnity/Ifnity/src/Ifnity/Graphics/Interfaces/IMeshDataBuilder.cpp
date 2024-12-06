
#include "IMeshDataBuilder.hpp"
#include "Ifnity\Graphics\Interfaces\IMeshObject.hpp"


IFNITY_NAMESPACE

void MeshDataBuilderCacheFile::buildMeshData(MeshObjectDescription& description)
{
	IFNITY_LOG(LogCore, WARNING, "Not implemented yet");

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


