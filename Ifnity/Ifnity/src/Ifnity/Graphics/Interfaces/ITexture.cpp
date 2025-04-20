
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

    // Check correct transform 
    const void* img = nullptr;
    switch(desc.dimension)
    {
    case rhi::TextureType::TEXTURECUBE:
        img = stbi_loadf(desc.filepath.c_str(), &w, &h, &comp, 3);
        break;
    case rhi::TextureType::TEXTURE2D:
		if(desc.format == rhi::Format::R8G8B8A8)
       img = stbi_load(desc.filepath.c_str(), &w, &h, &comp, STBI_rgb_alpha);
        else
        {
			img = stbi_load(desc.filepath.c_str(), &w, &h, &comp, STBI_rgb);
        }
		break;
    default:
        img = stbi_load(desc.filepath.c_str(), &w, &h, &comp, 3);
        break;
    }

    if(img == nullptr)
    {
        return nullptr;
    }

    desc.width = w;
    desc.height = h;
	desc.comp = comp;

    return img; 
	
}

 void FreeTexture(const void* img)
{
	stbi_image_free(const_cast<void*>(img));

}


 bool isDepthFormat(rhi::Format format)
 {
	 return properties   [SCAST_U8(format) ].depth 
            || properties[SCAST_U8(format)].stencil;
 }

 bool getNumImagePlanes(rhi::Format format)
 {
	 return properties[ SCAST_U8(format) ].numPlanes;
 }



 

IFNITY_END_NAMESPACE


