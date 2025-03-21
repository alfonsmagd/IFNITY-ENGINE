#pragma once

#include <pch.h>
#include <VkBootstrap.h>
#include "vulkan_ImmediateCommands.hpp"



IFNITY_NAMESPACE

class DeviceVulkan;

namespace Vulkan
{
	struct VulkanBuffer final
	{
		// clang-format off
		[[nodiscard]] inline uint8_t* getMappedPtr() const { return static_cast<uint8_t*>(mappedPtr_); }
		[[nodiscard]] inline bool isMapped() const { return mappedPtr_ != nullptr; }
		// clang-format on

		void bufferSubData(const DeviceVulkan& ctx, size_t offset, size_t size, const void* data);
		void getBufferSubData(const DeviceVulkan& ctx, size_t offset, size_t size, void* data);
		void flushMappedMemory(const DeviceVulkan& ctx, VkDeviceSize offset, VkDeviceSize size) const;
		void invalidateMappedMemory(const DeviceVulkan& ctx, VkDeviceSize offset, VkDeviceSize size) const;

	public:
		VkBuffer vkBuffer_ = VK_NULL_HANDLE;
		VkDeviceMemory vkMemory_ = VK_NULL_HANDLE;
		VmaAllocation vmaAllocation_ = VK_NULL_HANDLE;
		VkDeviceAddress vkDeviceAddress_ = 0;
		VkDeviceSize bufferSize_ = 0;
		VkBufferUsageFlags vkUsageFlags_ = 0;
		VkMemoryPropertyFlags vkMemFlags_ = 0;
		void* mappedPtr_ = nullptr;
		bool isCoherentMemory_ = false;

		static inline  VkBufferCreateInfo bufferCreateInfo()
		{
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			return bufferCreateInfo;
		}

	};

	
	

} // namespace Vulkan

IFNITY_END_NAMESPACE