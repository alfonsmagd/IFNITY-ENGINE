#pragma once 

//This file its based in 3D 3D Graphics Rendering Cookbook By : Sergey Kosarevsky, Viktor Latypov

#include "pch.h"

IFNITY_NAMESPACE

constexpr const uint32_t kMaxLODs = 8;  
constexpr const uint32_t kMaxStreams = 8;

// All offsets are relative to the beginning of the data block (excluding headers with Mesh list)
/**
 * @brief Represents a mesh with multiple levels of detail (LODs) and vertex data streams.
 */
struct  IFNITY_API Mesh final
{
	/** Number of LODs in this mesh. Strictly less than MAX_LODS, last LOD offset is used as a marker only */
	uint32_t lodCount = 1;

	/** Number of vertex data streams */
	uint32_t streamCount = 0;

	/** The total count of all previous vertices in this mesh file */
	uint32_t indexOffset = 0;

	uint32_t vertexOffset = 0;

	/** Vertex count (for all LODs) */
	uint32_t vertexCount = 0;

	/** Offsets to LOD data. Last offset is used as a marker to calculate the size */
	uint32_t lodOffset[ kMaxLODs ] = { 0 };

	/**
  * @brief Get the number of indices for a specific LOD.
  * @param lod The LOD level.
  * @return The number of indices for the specified LOD.
  */
	inline uint32_t getLODIndicesCount(uint32_t lod) const
	{
		assert(lod + 1 < kMaxLODs);
		return lodOffset[ lod + 1 ] - lodOffset[ lod ];
	}

	/** All the data "pointers" for all the streams */
	uint32_t streamOffset[ kMaxStreams ] = { 0 };

	/** Information about stream element (size pretty much defines everything else, the "semantics" is defined by the shader) */
	uint32_t streamElementSize[ kMaxStreams ] = { 0 };

	/** We could have included the streamStride[] array here to allow interleaved storage of attributes.
    For this book we assume tightly-packed (non-interleaved) vertex attribute streams */
	uint32_t  streamStride[ kMaxStreams ] = { 0 };

	/**
  * @brief Get the offset of a vertex attribute in a specific stream.
  * @param vertexIndex The index of the vertex.
  * @param stream The stream index.
  * @return The offset of the vertex attribute in the specified stream.
  */
	inline uint32_t getVertexAttributeOffset(uint32_t vertexIndex, uint32_t stream) const
	{
		assert(stream < kMaxStreams);
		return streamOffset[ stream ] + vertexIndex * streamStride[ stream ];
	}

	/**Additional information, like mesh name, can be added here */
};

/**
 * @brief Represents the header of a mesh file containing metadata and offsets to mesh data.
 */
struct IFNITY_API MeshFileHeader
{
	/* Unique 64-bit value to check integrity of the file */
	uint32_t magicValue;

	/* Number of mesh descriptors following this header */
	uint32_t meshCount;

	/* The offset to combined mesh data (this is the base from which the offsets in individual meshes start) */
	uint32_t dataBlockStartOffset;

	/* How much space index data takes */
	uint32_t indexDataSize;

	/* How much space vertex data takes */
	uint32_t vertexDataSize;

	/* According to your needs, you may add additional metadata fields */
};

struct IFNITY_API DrawData
{
	uint32_t meshIndex;
	uint32_t materialIndex;
	uint32_t LOD;
	uint32_t indexOffset;
	uint32_t vertexOffset;
	uint32_t transformIndex;
};

struct IFNITY_API MeshData
{
	std::vector<uint32_t> indexData_;
	std::vector<float> vertexData_;
	std::vector<Mesh> meshes_;
	
};

static_assert(sizeof(DrawData) == sizeof(uint32_t) * 6);

IFNITY_API MeshFileHeader loadMeshData(const char* filename, MeshData& out);

IFNITY_API void combineBuffers(const std::vector<uint32_t>& indexData, const std::vector<float>& vertexData, std::vector<uint8_t>& combinedBuffer);

IFNITY_API void loadCombinedBuffer(FILE* file, MeshFileHeader& header, std::vector<uint8_t>& combinedBuffer);

IFNITY_END_NAMESPACE