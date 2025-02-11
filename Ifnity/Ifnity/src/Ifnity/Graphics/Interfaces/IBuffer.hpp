#pragma once

#include "pch.h"
IFNITY_NAMESPACE


struct IFNITY_API VertexAttributeDescription
{
    uint32_t index = 0;
    uint32_t arraySize = 1;
    uint32_t offset = 0;
    uint32_t elementStride = 0;
    size_t size = 0;
    rhi::Format format = rhi::Format::UNKNOWN;
    rhi::VerxtexAttributeType type = rhi::VerxtexAttributeType::NONE;
    const void* data = nullptr;
    bool isInstanced = false;
    bool haveIndexBuffer = false;
    std::string name;

    VertexAttributeDescription&           setName(const std::string& value) { name = value; return *this; }
    constexpr VertexAttributeDescription& setFormat(rhi::Format value) { format = value; return *this; }
    constexpr VertexAttributeDescription& setArraySize(uint32_t value) { arraySize = value; return *this; }
    constexpr VertexAttributeDescription& setBufferIndexLocation(uint32_t value) { index = value; return *this; }
    constexpr VertexAttributeDescription& setOffset(uint32_t value) { offset = value; return *this; }
    constexpr VertexAttributeDescription& setElementStride(uint32_t value) { elementStride = value; return *this; }
    constexpr VertexAttributeDescription& setIsInstanced(bool value) { isInstanced = value; return *this; }
    constexpr VertexAttributeDescription& setSize(size_t value) { size = value; return *this; }
	constexpr VertexAttributeDescription& setData(const void* value) { data = value; return *this; }
	constexpr VertexAttributeDescription& setHaveIndexBuffer(bool value) { haveIndexBuffer = value; return *this; }
	constexpr VertexAttributeDescription& setAttributeType(rhi::VerxtexAttributeType value) { type = value; return *this; }
};




/**
 * @brief Enum class representing different types of buffers.
 */
enum class IFNITY_API BufferType: unsigned char
{
    DEFAULT_BUFFER = 0x00,    ///< Default buffer type
    VERTEX_BUFFER  = 0x01 ,    ///< Buffer for vertex data
    INDEX_BUFFER   = 0x02,     ///< Buffer for index data
	INDIRECT_BUFFER = 0x04,    ///< Buffer for indirect data
	VERTEX_INDEX_BUFFER = VERTEX_BUFFER | INDEX_BUFFER, ///< Buffer for vertex and index data
    CONSTANT_BUFFER = 0xF0,    ///< Buffer for constant data
	VERTEX_PULLING_BUFFER_INDEX = 0x10,///< Buffer for vertex pulling data this implies that the buffer is used for vertex pulling and configure the index buffer 
	VERTEX_PULLING_BUFFER = 0x20,///< Buffer for vertex pulling data , Vertex data, no index buffer.
	STORAGE_BUFFER = 0x30,     ///< Buffer for storage data

    NO_DEFINE_BUFFER = 0xFF ///< Undefined buffer type
};

/**
 * @brief Enum class representing different types of storage USING IN VK/D3D12 Opengl discard it.
 */
enum class IFNITY_API StorageType: unsigned char
{
    Device,
    HostVisible,
    Memoryless
};





/**
 * @brief Structure describing the properties of a buffer.
 */
struct IFNITY_API BufferDescription 
{
    uint64_t size = 0;          ///< Size of the buffer in bytes
    uint32_t strideSize = 0;        ///< Stride size of the buffer
	uint32_t offset = 0;            ///< Offset of the buffer
    std::string debugName;          ///< Debug name for the buffer
    BufferType type = BufferType::NO_DEFINE_BUFFER; ///< Type of the buffer
	uint8_t binding = 0;            ///< Binding point of the buffer

	const void* data ;     ///< Data to be written to the buffer


    /**
     * @brief Sets the byte size of the buffer.
     * @param size Size in bytes
     * @return Reference to the current BufferDescription object
     */
    constexpr BufferDescription& SetByteSize(uint64_t size) noexcept
    {
        size = size;
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

	/**
	 * @brief Sets the binding point of the buffer.
	 * @param bind Binding point
	 * @return Reference to the current BufferDescription object
	 */
    constexpr BufferDescription& SetBindingPoint(uint8_t bind) noexcept
    {
		binding = bind;
		return *this;

    }


	constexpr BufferDescription& SetOffset(uint32_t value) noexcept
	{
		offset = value;
		return *this;
	}

	constexpr BufferDescription& SetData(const void* value) noexcept
	{
		data = value;
		return *this;
	}

private:
	friend class IDevice;
    unsigned int meshIDasociate = -1;      ///< Associate mesh ID, for example in OPENGL this is the VAO ID

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
	virtual void SetData(const void* data) = 0;
	virtual const void* GetData() const = 0;
};

using  BufferHandle = std::shared_ptr<IBuffer>;

IFNITY_END_NAMESPACE