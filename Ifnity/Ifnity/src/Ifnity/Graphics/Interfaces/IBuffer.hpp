#pragma once

IFNITY_NAMESPACE

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