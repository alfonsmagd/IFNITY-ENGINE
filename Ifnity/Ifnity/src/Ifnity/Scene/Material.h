#pragma once

//Materals is based on the book 3D Graphics Rendering Cookbook by Sergey Kosarevsky and Viktor Latypov 

#include "pch.h"
#include "Ifnity/Utils/vec4.hpp"

#include <assimp\material.h>


IFNITY_NAMESPACE

enum MaterialFlags
{
	sMaterialFlags_CastShadow = 0x1,
	sMaterialFlags_ReceiveShadow = 0x2,
	sMaterialFlags_Transparent = 0x4,
};

constexpr const uint64_t INVALID_TEXTURE = 0xFFFFFFFF;

struct PACKED_STRUCT MaterialDescription final
{
	gpuvec4 emissiveColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	gpuvec4 albedoColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	// UV anisotropic roughness (isotropic lighting models use only the first value). ZW values are ignored
	gpuvec4 roughness_ = { 1.0f, 1.0f, 0.0f, 0.0f };
	float transparencyFactor_ = 1.0f;
	float alphaTest_ = 0.0f;
	float metallicFactor_ = 0.0f;
	uint32_t flags_ = sMaterialFlags_CastShadow | sMaterialFlags_ReceiveShadow;
	// maps
	uint64_t ambientOcclusionMap_ = INVALID_TEXTURE;
	uint64_t emissiveMap_ = INVALID_TEXTURE;
	uint64_t albedoMap_ = INVALID_TEXTURE;
	/// Occlusion (R), Roughness (G), Metallic (B) https://github.com/KhronosGroup/glTF/issues/857
	uint64_t metallicRoughnessMap_ = INVALID_TEXTURE;
	uint64_t normalMap_ = INVALID_TEXTURE;
	uint64_t opacityMap_ = INVALID_TEXTURE;

     bool operator==(const MaterialDescription& rhs) const
     {
		 //Check MaterialDescription for testing purposes
         return ambientOcclusionMap_ == rhs.ambientOcclusionMap_ &&
                emissiveMap_ == rhs.emissiveMap_ &&
                albedoMap_ == rhs.albedoMap_ &&
                metallicRoughnessMap_ == rhs.metallicRoughnessMap_ &&
                normalMap_ == rhs.normalMap_ &&
                // Añade más comparaciones según sea necesario
                true;
     }
};

static_assert(sizeof(MaterialDescription) % 16 == 0, "MaterialDescription should be padded to 16 bytes");

void saveMaterials(const char* fileName, const std::vector<MaterialDescription>& materials, const std::vector<std::string>& files);
void loadMaterials(const char* fileName, std::vector<MaterialDescription>& materials, std::vector<std::string>& files);

// Merge material lists from multiple scenes (follows the logic of merging in mergeScenes)
void mergeMaterialLists(
	// Input:
	const std::vector< std::vector<MaterialDescription>* >& oldMaterials, // all materials
	const std::vector< std::vector<std::string>* >& oldTextures,          // all textures from all material lists
	// Output:
	std::vector<MaterialDescription>& allMaterials,
	std::vector<std::string>& newTextures                                // all textures (merged from oldTextures, only unique items)
);



MaterialDescription convertAIMaterialToDescription(const aiMaterial* M, std::vector<std::string>& files, std::vector<std::string>& opacityMaps);

void convertAndDownscaleAllTextures(
	const std::vector<MaterialDescription>& materials, const std::string& basePath, std::vector<std::string>& files, std::vector<std::string>& opacityMaps
);

void dumpMaterial(const std::vector<std::string>& files, const MaterialDescription& d);


bool compareMaterials(const std::vector<MaterialDescription>& materials1, const std::vector<MaterialDescription>& materials2);

void checkMaterialsAndFiles(const std::vector<MaterialDescription>& materials, const std::vector<std::string>& files,
	const std::vector<MaterialDescription>& allMaterials, const std::vector<std::string>& files2);


IFNITY_END_NAMESPACE