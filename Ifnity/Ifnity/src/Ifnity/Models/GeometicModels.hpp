
#include "pch.h"

IFNITY_NAMESPACE

namespace GeometricModels
{
	// Estructura que define un vértice
	struct IFNITY_API Vertex
	{
		glm::vec3 position; // posición del vértice
		glm::vec3 normal;   // normal del vértice
		glm::vec3 tangent;  // tangente del vértice
	};


	struct IFNITY_API Tetrahedron
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> index;

		Tetrahedron();

		inline size_t GetSizeIndices() const { return index.size() * sizeof(unsigned int)	; }
		inline size_t GetSizeVertices() const { return vertices.size() * sizeof(Vertex); }


	};
};


IFNITY_END_NAMESPACE