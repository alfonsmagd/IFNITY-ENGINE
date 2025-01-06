#pragma once 
#include "pch.h"
#include "Ifnity\Graphics\VtxData.hpp"

IFNITY_NAMESPACE

namespace GeometricModels
{
	 //Estructura que define un vértice
	struct IFNITY_API VertexGeometry
	{
		glm::vec3 position; // posición del vértice
		glm::vec3 normal;   // normal del vértice
		glm::vec3 tangent;  // tangente del vértice
	};

	 //enum class type geometrical model 
	enum class  GeometricalModelType: uint8_t
	{
		TETHAHEDRON,
		CUBE,
		SPHERE,
		CYLYNDER,
		CONE,
		PLANE,

	};
	
	struct IFNITY_API GeometricModel
	{
		virtual ~GeometricModel() = default;
		std::vector<VertexGeometry> vertices;
		std::vector<uint32_t> index;
		inline const size_t GetSizeIndices() const { return index.size() * sizeof(unsigned int); }
		inline const size_t GetSizeVertices() const { return vertices.size() * sizeof(VertexGeometry); }

		 void toMeshData(MeshData& meshData)
		{
			// Reserve space in memory for the vertex and index data
			meshData.vertexData_.resize(vertices.size() * sizeof(VertexGeometry));
			meshData.indexData_.resize(index.size() * sizeof(uint32_t));

			// memcpy is used to copy the data from the vertices and index vectors to the meshData vertexData and indexData vectors
			memcpy(meshData.vertexData_.data(), vertices.data(), meshData.vertexData_.size());
			memcpy(meshData.indexData_.data(), index.data(), meshData.indexData_.size());

			//Empty the meshes. 
			meshData.meshes_.push_back(Mesh{});
		}
		
	};
	

	struct IFNITY_API Tetrahedron: public GeometricModel
	{
		Tetrahedron();
		//Destructor 
		~Tetrahedron() override;
		
		
	};

	struct IFNITY_API Cube: public GeometricModel
	{
		Cube();
		//Destructor 
		~Cube() override;
	
	};

};


IFNITY_END_NAMESPACE