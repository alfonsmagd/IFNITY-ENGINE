# FindGraphicsAPIs.cmake


#Try compile D3D12 example
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
        message(STATUS "D3D12 IS NOT SUPPORTED. ${D3D12_COMPILE_OUTPUT}")
    endif()

    if(D3D11_COMPILE_SUCCEEDED)
        message(STATUS "D3D11 is supported")
    else()
        message(STATUS "D3D11 IS NOT SUPPORTED. ${D3D11_COMPILE_OUTPUT}")
    endif()
endif()

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)



if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan was not found on the system.")
else()
    message(STATUS "Vulkan found: ${Vulkan_LIBRARY}")
endif()

if(NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL was not found on the system.")
else()
    message(STATUS "OpenGL found: ${OpenGL_LIBRARY}")
endif()
