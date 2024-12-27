

#include "Material.h"

//Include Assimp headers.
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

//stl
#include <unordered_map>
#include <execution>

#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize2.h>

//This will be added to util functions in the future
inline int addUnique(std::vector<std::string>& files, const std::string& file)
{
	if(file.empty())
		return -1;

	auto i = std::find(std::begin(files), std::end(files), file);

	if(i == files.end())
	{
		files.push_back(file);
		return (int)files.size() - 1;
	}

	return (int)std::distance(files.begin(), i);
}

void saveStringList(FILE* f, const std::vector<std::string>& lines)
{
	uint32_t sz = (uint32_t)lines.size();
	fwrite(&sz, sizeof(uint32_t), 1, f);
	for(const auto& s : lines)
	{
		sz = (uint32_t)s.length();
		fwrite(&sz, sizeof(uint32_t), 1, f);
		fwrite(s.c_str(), sz + 1, 1, f);
	}
}

void loadStringList(FILE* f, std::vector<std::string>& lines)
{
	{
		uint32_t sz = 0;
		fread(&sz, sizeof(uint32_t), 1, f);
		lines.resize(sz);
	}
	std::vector<char> inBytes;
	for(auto& s : lines)
	{
		uint32_t sz = 0;
		fread(&sz, sizeof(uint32_t), 1, f);
		inBytes.resize(sz + 1);
		fread(inBytes.data(), sz + 1, 1, f);
		s = std::string(inBytes.data());
	}
}

void saveMaterials(const char* fileName, const std::vector<MaterialDescription>& materials, const std::vector<std::string>& files)
{
	FILE* f = fopen(fileName, "wb");
	if(!f)
		return;

	uint32_t sz = (uint32_t)materials.size();
	fwrite(&sz, 1, sizeof(uint32_t), f);
	fwrite(materials.data(), sizeof(MaterialDescription), sz, f);
	saveStringList(f, files);
	fclose(f);
}

void loadMaterials(const char* fileName, std::vector<MaterialDescription>& materials, std::vector<std::string>& files)
{
	FILE* f = fopen(fileName, "rb");
	if(!f)
	{
		printf("Cannot load file %s\nPlease run SceneConverter tool from Chapter7\n", fileName);
		exit(255);
	}

	uint32_t sz;
	fread(&sz, 1, sizeof(uint32_t), f);
	materials.resize(sz);
	fread(materials.data(), sizeof(MaterialDescription), materials.size(), f);
	loadStringList(f, files);
	fclose(f);
}

void mergeMaterialLists(
	const std::vector< std::vector<MaterialDescription>* >& oldMaterials,
	const std::vector< std::vector<std::string>* >& oldTextures,
	std::vector<MaterialDescription>& allMaterials,
	std::vector<std::string>& newTextures
)
{
	// map texture names to indices in newTexturesList (calculated as we fill the newTexturesList)
	std::unordered_map<std::string, int> newTextureNames;
	std::unordered_map<int, int> materialToTextureList; // direct MaterialDescription usage as a key is impossible, so we use its index in the allMaterials array

	// Create combined material list [no hashing of materials, just straightforward merging of all lists]
	int midx = 0;
	for(const std::vector<MaterialDescription>* ml : oldMaterials)
	{
		for(const MaterialDescription& m : *ml)
		{
			allMaterials.push_back(m);
			materialToTextureList[ allMaterials.size() - 1 ] = midx;
		}

		midx++;
	}

	// Create one combined texture list
	for(const std::vector<std::string>* tl : oldTextures)
		for(const std::string& file : *tl)
		{
			newTextureNames[ file ] = addUnique(newTextures, file); // addUnique() is in SceneConverter/MaterialConv.inl
		}

	// Lambda to replace textureID by a new "version" (from global list)
	auto replaceTexture = [ &materialToTextureList, &oldTextures, &newTextureNames ](int m, uint64_t* textureID)
		{
			if(*textureID < INVALID_TEXTURE)
			{
				auto listIdx = materialToTextureList[ m ];
				auto texList = oldTextures[ listIdx ];
				const std::string& texFile = (*texList)[ *textureID ];
				*textureID = (uint64_t)(newTextureNames[ texFile ]);
			}
		};

	for(size_t i = 0; i < allMaterials.size(); i++)
	{
		auto& m = allMaterials[ i ];
		replaceTexture(i, &m.ambientOcclusionMap_);
		replaceTexture(i, &m.emissiveMap_);
		replaceTexture(i, &m.albedoMap_);
		replaceTexture(i, &m.metallicRoughnessMap_);
		replaceTexture(i, &m.normalMap_);
	}
}





float getMaterialFloat(const aiMaterial* M, const char* key, unsigned int type, unsigned int idx, float defaultValue)
{
	float value = defaultValue;
	aiGetMaterialFloat(M, key, type, idx, &value);
	return value;
}




MaterialDescription convertAIMaterialToDescription(const aiMaterial* M, std::vector<std::string>& files, std::vector<std::string>& opacityMaps)
{
	MaterialDescription D;

	aiColor4D Color;

	if(aiGetMaterialColor(M, AI_MATKEY_COLOR_AMBIENT, &Color) == AI_SUCCESS)
	{
		D.emissiveColor_ = { Color.r, Color.g, Color.b, Color.a };
		if(D.emissiveColor_.w > 1.0f) D.emissiveColor_.w = 1.0f; // clamp alpha
	}
	if(aiGetMaterialColor(M, AI_MATKEY_COLOR_DIFFUSE, &Color) == AI_SUCCESS)
	{
		D.albedoColor_ = { Color.r, Color.g, Color.b, Color.a };
		if(D.albedoColor_.w > 1.0f) D.albedoColor_.w = 1.0f; // clamp alpha
	}
	if(aiGetMaterialColor(M, AI_MATKEY_COLOR_EMISSIVE, &Color) == AI_SUCCESS)
	{
		D.emissiveColor_.x += Color.r;
		D.emissiveColor_.y += Color.g;
		D.emissiveColor_.z += Color.b;
		D.emissiveColor_.w += Color.a;
		if(D.emissiveColor_.w > 1.0f) D.albedoColor_.w = 1.0f; // clamp alpha
	}

	const float opaquenessThreshold = 0.05f;
	float Opacity = 1.0f;

	//Get the opacity of the material and convert it to transparency factor
	if(aiGetMaterialFloat(M, AI_MATKEY_OPACITY, &Opacity) == AI_SUCCESS)
	{
		D.transparencyFactor_ = glm::clamp(1.0f - Opacity, 0.0f, 1.0f);
		if(D.transparencyFactor_ >= 1.0f - opaquenessThreshold) D.transparencyFactor_ = 0.0f;
	}

	//Get transparency color and convert it to transparency factor
	if(aiGetMaterialColor(M, AI_MATKEY_COLOR_TRANSPARENT, &Color) == AI_SUCCESS)
	{
		const float Opacity = std::max(std::max(Color.r, Color.g), Color.b);
		D.transparencyFactor_ = glm::clamp(Opacity, 0.0f, 1.0f);
		if(D.transparencyFactor_ >= 1.0f - opaquenessThreshold) D.transparencyFactor_ = 0.0f;
		D.alphaTest_ = 0.5f;
	}

	float tmp = 1.0f;
	//Get Metallic factor 
	if(aiGetMaterialFloat(M, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, &tmp) == AI_SUCCESS)
		D.metallicFactor_ = tmp;
	//Get Roughness factor
	if(aiGetMaterialFloat(M, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, &tmp) == AI_SUCCESS)
		D.roughness_ = { tmp, tmp, tmp, tmp };

	aiString Path;
	aiTextureMapping Mapping;
	unsigned int UVIndex = 0;
	float Blend = 1.0f;
	aiTextureOp TextureOp = aiTextureOp_Add;
	aiTextureMapMode TextureMapMode[ 2 ] = { aiTextureMapMode_Wrap, aiTextureMapMode_Wrap };
	unsigned int TextureFlags = 0;

	if(aiGetMaterialTexture(M, aiTextureType_EMISSIVE, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp, TextureMapMode, &TextureFlags) == AI_SUCCESS)
	{
		D.emissiveMap_ = addUnique(files, Path.C_Str());
	}

	if(aiGetMaterialTexture(M, aiTextureType_DIFFUSE, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp, TextureMapMode, &TextureFlags) == AI_SUCCESS)
	{
		D.albedoMap_ = addUnique(files, Path.C_Str());
		const std::string albedoMap = std::string(Path.C_Str());
		if(albedoMap.find("grey_30") != albedoMap.npos)
			D.flags_ |= sMaterialFlags_Transparent;
	}

	// first try tangent space normal map
	if(aiGetMaterialTexture(M, aiTextureType_NORMALS, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp, TextureMapMode, &TextureFlags) == AI_SUCCESS)
	{
		D.normalMap_ = addUnique(files, Path.C_Str());
	}
	// then height map
	if(D.normalMap_ == 0xFFFFFFFF)
		if(aiGetMaterialTexture(M, aiTextureType_HEIGHT, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp, TextureMapMode, &TextureFlags) == AI_SUCCESS)
			D.normalMap_ = addUnique(files, Path.C_Str());

	if(aiGetMaterialTexture(M, aiTextureType_OPACITY, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp, TextureMapMode, &TextureFlags) == AI_SUCCESS)
	{
		D.opacityMap_ = addUnique(opacityMaps, Path.C_Str());
		D.alphaTest_ = 0.5f;
	}

	// patch materials
	aiString Name;
	std::string materialName;
	if(aiGetMaterialString(M, AI_MATKEY_NAME, &Name) == AI_SUCCESS)
	{
		materialName = Name.C_Str();
	}
	// apply heuristics
	if((materialName.find("Glass") != std::string::npos) ||
		(materialName.find("Vespa_Headlight") != std::string::npos))
	{
		D.alphaTest_ = 0.75f;
		D.transparencyFactor_ = 0.1f;
		D.flags_ |= sMaterialFlags_Transparent;
	}
	else if(materialName.find("Bottle") != std::string::npos)
	{
		D.alphaTest_ = 0.54f;
		D.transparencyFactor_ = 0.4f;
		D.flags_ |= sMaterialFlags_Transparent;
	}
	else if(materialName.find("Metal") != std::string::npos)
	{
		D.metallicFactor_ = 1.0f;
		D.roughness_ = gpuvec4(0.1f, 0.1f, 0.0f, 0.0f);
	}

	return D;
}


inline std::string replaceAll(const std::string& str, const std::string& oldSubStr, const std::string& newSubStr)
{
	std::string result = str;

	for(size_t p = result.find(oldSubStr); p != std::string::npos; p = result.find(oldSubStr))
		result.replace(p, oldSubStr.length(), newSubStr);

	return result;
}

/* Convert 8-bit ASCII string to upper case */
inline std::string lowercaseString(const std::string& s)
{
	std::string out(s.length(), ' ');
	std::transform(s.begin(), s.end(), out.begin(), tolower);
	return out;
}

/* find a file in directory which "almost" coincides with the origFile (their lowercase versions coincide) */
std::string findSubstitute(const std::string& origFile)
{
	namespace fs = std::filesystem;

	// Make absolute path
	auto apath = fs::absolute(fs::path(origFile));
	// Absolute lowercase filename [we compare with it]
	auto afile = lowercaseString(apath.filename().string());
	// Directory where in which the file should be
	auto dir = fs::path(origFile).remove_filename();

	// Iterate each file non-recursively and compare lowercase absolute path with 'afile'
	for(auto& p : fs::directory_iterator(dir))
		if(afile == lowercaseString(p.path().filename().string()))
			return p.path().string();

	return std::string{};
}

std::string fixTextureFile(const std::string& file)
{
	namespace fs = std::filesystem;
	// TODO: check the findSubstitute() function
	return fs::exists(file) ? file : findSubstitute(file);
}

std::string convertTexture(const std::string& file, const std::string& basePath, std::unordered_map<std::string, uint32_t>& opacityMapIndices, const std::vector<std::string>& opacityMaps)
{
	const int maxNewWidth = 512;
	const int maxNewHeight = 512;

	const auto srcFile = replaceAll(basePath + file, "\\", "/");
	const auto newFile = std::string("data/out_textures/") + lowercaseString(replaceAll(replaceAll(srcFile, "..", "__"), "/", "__") + std::string("__rescaled")) + std::string(".png");

	// load this image
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(fixTextureFile(srcFile).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	uint8_t* src = pixels;
	texChannels = STBI_rgb_alpha;

	std::vector<uint8_t> tmpImage(maxNewWidth * maxNewHeight * 4);

	if(!src)
	{
		printf("Failed to load [%s] texture\n", srcFile.c_str());
		texWidth = maxNewWidth;
		texHeight = maxNewHeight;
		texChannels = STBI_rgb_alpha;
		src = tmpImage.data();
	}
	else
	{
		printf("Loaded [%s] %dx%d texture with %d channels\n", srcFile.c_str(), texWidth, texHeight, texChannels);
	}

	if(opacityMapIndices.count(file) > 0)
	{
		const auto opacityMapFile = replaceAll(basePath + opacityMaps[ opacityMapIndices[ file ] ], "\\", "/");
		int opacityWidth, opacityHeight;
		stbi_uc* opacityPixels = stbi_load(fixTextureFile(opacityMapFile).c_str(), &opacityWidth, &opacityHeight, nullptr, 1);

		if(!opacityPixels)
		{
			printf("Failed to load opacity mask [%s]\n", opacityMapFile.c_str());
		}

		assert(opacityPixels);
		assert(texWidth == opacityWidth);
		assert(texHeight == opacityHeight);

		// store the opacity mask in the alpha component of this image
		if(opacityPixels)
			for(int y = 0; y != opacityHeight; y++)
				for(int x = 0; x != opacityWidth; x++)
					src[ (y * opacityWidth + x) * texChannels + 3 ] = opacityPixels[ y * opacityWidth + x ];

		stbi_image_free(opacityPixels);
	}

	const uint32_t imgSize = texWidth * texHeight * texChannels;
	std::vector<uint8_t> mipData(imgSize);
	uint8_t* dst = mipData.data();

	const int newW = std::min(texWidth, maxNewWidth);
	const int newH = std::min(texHeight, maxNewHeight);

	stbir_resize_uint8_linear(src, texWidth, texHeight, 0, dst, newW, newH, 0, (stbir_pixel_layout)texChannels);

	stbi_write_png(newFile.c_str(), newW, newH, texChannels, dst, 0);

	if(pixels)
		stbi_image_free(pixels);

	return newFile;
}

void convertAndDownscaleAllTextures(
	const std::vector<MaterialDescription>& materials, const std::string& basePath, std::vector<std::string>& files, std::vector<std::string>& opacityMaps
)
{
	std::unordered_map<std::string, uint32_t> opacityMapIndices(files.size());

	for(const auto& m : materials)
		if(m.opacityMap_ != 0xFFFFFFFF && m.albedoMap_ != 0xFFFFFFFF)
			opacityMapIndices[ files[ m.albedoMap_ ] ] = (uint32_t)m.opacityMap_;

	auto converter = [ & ](const std::string& s) -> std::string
		{
			return convertTexture(s, basePath, opacityMapIndices, opacityMaps);
		};

	std::transform(std::execution::par, std::begin(files), std::end(files), std::begin(files), converter);
}