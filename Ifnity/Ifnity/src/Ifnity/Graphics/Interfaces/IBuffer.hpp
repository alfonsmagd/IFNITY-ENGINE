#pragma once

#include "pch.h"
IFNITY_NAMESPACE


struct IFNITY_API VertexAttributeDescription
{
    std::string name;
    rhi::Format format = rhi::Format::UNKNOWN;
    uint32_t arraySize = 1;
    uint32_t bufferLocation = 0;
    uint32_t offset = 0;
    // note: for most APIs, all strides for a given bufferIndex must be identical
    uint32_t elementStride = 0;
    size_t   size = 0;
    bool isInstanced = false;

    VertexAttributeDescription& setName(const std::string& value) { name = value; return *this; }
    constexpr VertexAttributeDescription& setFormat(rhi::Format value) { format = value; return *this; }
    constexpr VertexAttributeDescription& setArraySize(uint32_t value) { arraySize = value; return *this; }
    constexpr VertexAttributeDescription& setBufferIndexLocation(uint32_t value) { bufferLocation = value; return *this; }
    constexpr VertexAttributeDescription& setOffset(uint32_t value) { offset = value; return *this; }
    constexpr VertexAttributeDescription& setElementStride(uint32_t value) { elementStride = value; return *this; }
    constexpr VertexAttributeDescription& setIsInstanced(bool value) { isInstanced = value; return *this; }
    constexpr VertexAttributeDescription& setSize(size_t value) { size = value; return *this; }
};




/**
 * @brief Enum class representing different types of buffers.
 */
enum class IFNITY_API BufferType: unsigned char
{
    VERTEX_BUFFER,      ///< Buffer for vertex data
    INDEX_BUFFER,       ///< Buffer for index data
    CONSTANT_BUFFER,    ///< Buffer for constant data

    NO_DEFINE_BUFFER = 0xFF ///< Undefined buffer type
};





/**
 * @brief Structure describing the properties of a buffer.
 */
struct IFNITY_API BufferDescription 
{
    uint64_t byteSize = 0;          ///< Size of the buffer in bytes
    uint32_t strideSize = 0;        ///< Stride size of the buffer
    std::string debugName;          ///< Debug name for the buffer
    BufferType type = BufferType::NO_DEFINE_BUFFER; ///< Type of the buffer

    /**
     * @brief Sets the byte size of the buffer.
     * @param size Size in bytes
     * @return Reference to the current BufferDescription object
     */
    constexpr BufferDescription& SetByteSize(uint64_t size) noexcept
    {
        byteSize = size;
        return *this;
    }

    /**
     * @brief Sets the stride size of the buffer.
     * @param size Stride size
     * @return Reference to the current BufferDescription object
     */
    constexpr BufferDescription& SetStrideSize(uint32_t size) noexcept
    {
        strideSize = size;
        return *this;
    }

    /**
     * @brief Sets the debug name of the buffer.
     * @param name Debug name
     * @return Reference to the current BufferDescription object
     */
    BufferDescription& SetDebugName(const std::string& name) noexcept
    {
        debugName = name;
        return *this;
    }

    /**
     * @brief Sets the type of the buffer.
     * @param bufferType Type of the buffer
     * @return Reference to the current BufferDescription object
     */
    constexpr BufferDescription& SetBufferType(BufferType bufferType) noexcept
    {
        type = bufferType;
        return *this;
    }
};

/**
 * @brief Interface for buffer objects.
 */
class IFNITY_API IBuffer
{
public:
    virtual ~IBuffer() = default;

    /**
     * @brief Gets the description of the buffer.
     * @return Reference to the BufferDescription object
     */
    virtual BufferDescription& GetBufferDescription() = 0;
	virtual const uint32_t GetBufferID() const = 0;
};

using BufferHandle = std::shared_ptr<IBuffer>;

IFNITY_END_NAMESPACE