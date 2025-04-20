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

/**
 * @brief Represents the data required to draw a mesh.
 */
struct IFNITY_API DrawData
{
	uint32_t meshIndex;       /**< Index of the mesh to draw */
	uint32_t materialIndex;   /**< Index of the material to use */
	uint32_t LOD;             /**< Level of detail to use */
	uint32_t indexOffset;     /**< Offset to the index data */
	uint32_t vertexOffset;    /**< Offset to the vertex data */
	uint32_t transformIndex;  /**< Index of the transform to apply */
};

struct IFNITY_API DrawID
{
	uint32_t transformId;
	uint32_t materialId;
};

/**
 * @brief Represents the data of a mesh including indices, vertices, and mesh descriptors.
 */
struct IFNITY_API MeshData
{
	std::vector<uint32_t> indexData_; /**< Index data */
	std::vector<float> vertexData_;   /**< Vertex data */
	std::vector<Mesh> meshes_;        /**< Mesh descriptors */
};

static_assert(sizeof(DrawData) == sizeof(uint32_t) * 6);

/**
 * @brief Loads mesh data from a file.
 * @param filename The name of the file to load.
 * @param out The output MeshData structure.
 * @return The MeshFileHeader containing metadata about the loaded mesh data.
 */
IFNITY_API MeshFileHeader loadMeshData(const char* filename, MeshData& out);

/**
 * @brief Combines index and vertex data into a single buffer.
 * @param indexData The index data to combine.
 * @param vertexData The vertex data to combine.
 * @param combinedBuffer The output combined buffer.
 */
IFNITY_API void combineBuffers(const std::vector<uint32_t>& indexData, const std::vector<float>& vertexData, std::vector<uint8_t>& combinedBuffer);

/**
 * @brief Loads a combined buffer from a file.
 * @param file The file to load from.
 * @param header The MeshFileHeader containing metadata about the mesh data.
 * @param combinedBuffer The output combined buffer.
 */
IFNITY_API void loadCombinedBuffer(FILE* file, MeshFileHeader& header, std::vector<uint8_t>& combinedBuffer);

/**
 * @brief Saves mesh data to a file.
 * @param filename The name of the file to save to.
 * @param data The MeshData to save.
 * @param fheader The MeshFileHeader containing metadata about the mesh data.
 */
IFNITY_API  void  saveMeshData(const char* filename, const MeshData& data, MeshFileHeader& fheader);

/**
 * @brief Saves mesh data to a file. in this case the header 
 * isnt not saved , its only use to write the header data , and then if you
 * want get the MeshFileHeader you need loadMeshData to get it.
 * 
 * 
 * @param filename The name of the file to save to.
 * @param data The MeshData to save.
 */
IFNITY_API  void saveMeshData(const char* filename, const MeshData& data);

IFNITY_END_NAMESPACE
