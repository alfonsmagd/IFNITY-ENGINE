#pragma once 
#include "pch.h"
#include "Ifnity\Graphics\VtxData.hpp"

IFNITY_NAMESPACE

namespace GeometricModels
{
	 //Estructura que define un v�rtice
	struct IFNITY_API Vertex
	{
		glm::vec3 position; // posici�n del v�rtice
		glm::vec3 normal;   // normal del v�rtice
		glm::vec3 tangent;  // tangente del v�rtice
	};

	
	struct IFNITY_API GeometricModel
	{
		virtual ~GeometricModel() = default;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> index;
		inline size_t GetSizeIndices() const { return index.size() * sizeof(unsigned int); }
		inline size_t GetSizeVertices() const { return vertices.size() * sizeof(Vertex); }

		virtual void toMeshData( MeshData& meshData) const = 0;
		
	};
	

	struct IFNITY_API Tetrahedron: public GeometricModel
	{
		Tetrahedron();

		void toMeshData(MeshData& meshData) const override;
		
	};
};


IFNITY_END_NAMESPACE