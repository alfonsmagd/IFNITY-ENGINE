#include "GeometicModels.hpp"


IFNITY_NAMESPACE

namespace GeometricModels
{
    Tetrahedron::Tetrahedron()
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
            0, 1, 2, // Cara 1
            0, 3, 1, // Cara 2
            0, 2, 3, // Cara 3
            1, 3, 2  // Cara 4
        };



    }

	void Tetrahedron::toMeshData(MeshData& meshData) const
	{
		// Reserve space in memory for the vertex and index data
		meshData.vertexData_.resize(vertices.size() * sizeof(Vertex));
		meshData.indexData_.resize(index.size() * sizeof(uint32_t));

		// memcpy is used to copy the data from the vertices and index vectors to the meshData vertexData and indexData vectors
		memcpy(meshData.vertexData_.data(), vertices.data(), meshData.vertexData_.size());
		memcpy(meshData.indexData_.data(), index.data(), meshData.indexData_.size());

        //Empty the meshes. 
        meshData.meshes_.push_back(Mesh{});

	}

}

IFNITY_END_NAMESPACE
