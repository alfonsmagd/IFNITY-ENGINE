
#pragma once

#pragma warning(disable : 4251) // disable warning for dll interface using in the same interface dll, if you want to use this dll in another dll you should use the same compiler and the same version of the compiler



#define SANDBOX_TOOL 
#define VMA_ALLOCATOR_VK 1

#if defined(_WIN32) || defined(_WIN64)
#if defined(BUILD_SHARED_IFNITY)
#define IFNITY_API __declspec(dllexport)
#elif defined(IFNITY_USE_SHARED)
#define IFNITY_API __declspec(dllimport)
#else
#define IFNITY_API
#endif
#else
#define IFNITY_API
#endif


#if defined(_MSC_VER) // Microsoft Visual Studio
#define IFNITY_INLINE __forceinline
#elif defined(__GNUC__) // GCC
#define IFNITY_INLINE __attribute__((always_inline))
#elif defined(__CLANG__) // Clang
#define IFNITY_INLINE __attribute__((always_inline))
#else
#define IFNITY_INLINE
#endif

#define BIT(X) (1 << X)
#define IFNITY_NAMESPACE namespace IFNITY {
#define IFNITY_END_NAMESPACE }


#define IN      // This help you to understand that the variable is input in the function.
#define OUT     // This help you to understand that the variable change in the function AND ITS OUTPUT


#define ARRAY_NUM_ELEMENTS(array) (sizeof(array) / sizeof(array[0]))
#define SCAST_U8(x) static_cast<uint8_t>(x) 
#define SCAST_U16(x) static_cast<uint16_t>(x)
#define DCAST_BUFFER(handle)  dynamic_cast<Buffer*>(handle)
#define DCAST_DEVICE(handle)  dynamic_cast<Device*>(handle)
#define DCAST_TEXTURE(handle) dynamic_cast<Texture*>(handle)


//Templates Concepts
template<typename T>
concept PushConstentImpl = requires { sizeof(T); };
