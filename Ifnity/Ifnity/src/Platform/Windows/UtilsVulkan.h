
#pragma once

#include  "pch.h"

#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>


#define VK_CHECK(result, errorMessage) \
    if ((result) != VK_SUCCESS) { \
        IFNITY_LOG(LogCore, ERROR, errorMessage); \
        \
    }



struct ImageBlock
{
	VkImage			image = VK_NULL_HANDLE;
	VkImageView     imageView = VK_NULL_HANDLE;
	VkFormat		format;
	VmaAllocation	imageAlloc = VK_NULL_HANDLE;
	VkDeviceMemory  imageMemory = VK_NULL_HANDLE;
};