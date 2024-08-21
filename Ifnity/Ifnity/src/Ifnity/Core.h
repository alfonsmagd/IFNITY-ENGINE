
#pragma once

#pragma warning(disable : 4251) // disable warning for dll interface using in the same interface dll, if you want to use this dll in another dll you should use the same compiler and the same version of the compiler

#ifdef IFNITY_EXPORTS_DLL
#define IFNITY_API __declspec(dllexport)
#else
#define IFNITY_API __declspec(dllimport)
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

