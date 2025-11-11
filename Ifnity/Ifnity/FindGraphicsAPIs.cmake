# FindGraphicsAPIs.cmake

# Check D3D12 availability
if(WIN32)
    try_compile(D3D12_COMPILE_SUCCEEDED
        "${PROJECT_SOURCE_DIR}/CmakeCheck"
        "${PROJECT_SOURCE_DIR}/CmakeCheck/CheckD3D12.cpp"
        OUTPUT_VARIABLE D3D12_COMPILE_OUTPUT
    )
    try_compile(D3D11_COMPILE_SUCCEEDED
        "${PROJECT_SOURCE_DIR}/CmakeCheck"
        "${PROJECT_SOURCE_DIR}/CmakeCheck/CheckD3D11.cpp"
        OUTPUT_VARIABLE D3D11_COMPILE_OUTPUT
    )
    if(D3D12_COMPILE_SUCCEEDED)
        message(STATUS "D3D12 is supported.")
    else()
        message(WARNING "D3D12 IS NOT SUPPORTED. Disabling D3D12 backend.")
        set(IFNITY_D3D12_API OFF CACHE BOOL "Enable D3D12 backend" FORCE)
    endif()

    if(D3D11_COMPILE_SUCCEEDED)
        message(STATUS "D3D11 is supported")
    else()
        message(STATUS "D3D11 IS NOT SUPPORTED.")
    endif()
else()
    # D3D12 is Windows-only
    if(IFNITY_D3D12_API)
        message(WARNING "D3D12 is only supported on Windows. Disabling D3D12 backend.")
        set(IFNITY_D3D12_API OFF CACHE BOOL "Enable D3D12 backend" FORCE)
    endif()
endif()

# Find Vulkan (optional)
find_package(Vulkan)

if(NOT Vulkan_FOUND)
    message(WARNING "Vulkan was not found on the system. Disabling Vulkan backend.")
    set(IFNITY_VULKAN_API OFF CACHE BOOL "Enable Vulkan backend" FORCE)
else()
    message(STATUS "Vulkan found: ${Vulkan_LIBRARY}")
endif()

# Find OpenGL (optional)
find_package(OpenGL)

if(NOT OpenGL_FOUND)
    message(WARNING "OpenGL was not found on the system. Disabling OpenGL backend.")
    set(IFNITY_OPENGL_API OFF CACHE BOOL "Enable OpenGL backend" FORCE)
else()
    message(STATUS "OpenGL found: ${OPENGL_LIBRARIES}")
endif()

# Verify at least one graphics API is available
if(NOT IFNITY_OPENGL_API AND NOT IFNITY_VULKAN_API AND NOT IFNITY_D3D12_API)
    message(FATAL_ERROR "No graphics API is available. At least one of OpenGL, Vulkan, or D3D12 must be supported.")
endif()
