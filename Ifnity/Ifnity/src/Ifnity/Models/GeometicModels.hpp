
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

		Tetrahedron()
		{
			vertices =
			{
				// Vértices del tetraedro con sus posiciones, normales y tangentes
				{{1.0f, 1.0f, 1.0f},   {0.577f, 0.577f, 0.577f},   {1.0f, 0.0f, 0.0f}},
				{{-1.0f, -1.0f, 1.0f}, {-0.577f, -0.577f, 0.577f}, {0.0f, 1.0f, 0.0f}},
				{{-1.0f, 1.0f, -1.0f}, {-0.577f, 0.577f, -0.577f}, {0.0f, 0.0f, 1.0f}},
				{{1.0f, -1.0f, -1.0f}, {0.577f, -0.577f, -0.577f}, {1.0f, 1.0f, 0.0f}},
			};

			index =
			{
				0, 1, 2, // Face 1
				0, 3, 1, // Face 2
				0, 2, 3, // Face 3
				1, 3, 2  // Face 4
			};
		}

		size_t GetSizeIndices() const { return index.size() * sizeof(unsigned int)	; }
		size_t GetSizeVertices() const { return vertices.size() * sizeof(Vertex); }


	};
};


IFNITY_END_NAMESPACE