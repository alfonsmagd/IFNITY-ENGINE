
#include "ITexture.hpp"

#include "../vendor/stb_image/stb_image.h"


IFNITY_NAMESPACE

const void* LoadTextureFromFile(const std::string& filePath)
{
    int w, h, comp;
    const uint8_t* img = stbi_load(filePath.c_str(), &w, &h, &comp, 3);

    if(img == nullptr)
    {
        return nullptr;
    }

    return static_cast<const void*>(img);
}



const void* LoadTextureFromFileDescription(TextureDescription& desc)
{

    int w, h, comp;
    const uint8_t* img = stbi_load(desc.filepath.c_str(), &w, &h, &comp, 3);

    if(img == nullptr)
    {
        return nullptr;
    }
    desc.width = w;
    desc.height = h;

    return static_cast<const void*>(img);
	
}

void FreeTexture(const void* img)
{}

IFNITY_END_NAMESPACE


