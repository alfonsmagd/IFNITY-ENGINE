
#pragma once

#pragma warning(disable : 4251) // disable warning for dll interface using in the same interface dll, if you want to use this dll in another dll you should use the same compiler and the same version of the compiler



#define SANDBOX_TOOL 

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
#define OUT    // This help you to understand that the variable change in the function.


#define ARRAY_NUM_ELEMENTS(array) (sizeof(array) / sizeof(array[0]))



