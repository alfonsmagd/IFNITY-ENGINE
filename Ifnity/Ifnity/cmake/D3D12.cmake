# DirectX 12 Configuration - Platform-specific D3D12 backend
message(STATUS "Configuring DirectX 12 backend components...")

# ========= D3D12 BACKEND SOURCES =========
set(D3D12_SOURCES)
set(D3D12_HEADERS)

# Platform-specific D3D12 files
file(GLOB_RECURSE D3D12_PLATFORM_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/d3d12_backend.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/DeviceD3D12.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/DeviceD3D11.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/D3D12/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/D3D12/*.c"
)

file(GLOB_RECURSE D3D12_PLATFORM_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/d3d12_backend.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/DeviceD3D12.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/DeviceD3D11.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/D3D12/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/D3D12/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/D3D12/d3dx12.h"

)

# D3D12 ImGui rendering backend
file(GLOB_RECURSE D3D12_IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/ImguiRender/ImguiD3D12Render.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/ImguiRender/ImguiD3D11Render.cpp"

)

file(GLOB_RECURSE D3D12_IMGUI_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/ImguiRender/ImguiD3D12Render.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/ImguiRender/ImguiD3D11Render.h"
)

# Combine D3D12 files
list(APPEND D3D12_SOURCES 
    ${D3D12_PLATFORM_SOURCES}
    ${D3D12_IMGUI_SOURCES}
)

list(APPEND D3D12_HEADERS
    ${D3D12_PLATFORM_HEADERS}
    ${D3D12_IMGUI_HEADERS}
)

# ========= D3D12 VENDOR DEPENDENCIES =========
# D3D12-specific vendor dependencies
set(D3D12_VENDOR_SOURCES
    # D3D12 Memory Allocator
    # DXC Compiler integration
    # These are handled by add_subdirectory and linking in main CMakeLists.txt
)

list(APPEND D3D12_SOURCES ${D3D12_VENDOR_SOURCES})

# ========= D3D12 INCLUDES =========
function(setup_d3d12_includes target)
    target_include_directories(${target} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/D3D12MemoryAllocator/include"
        "${DXC_INCLUDE_DIR}"
        "${DX12_AGILITY_SDK_PATH}/include"
    )
    target_include_directories(${target} BEFORE PRIVATE "${DX12_AGILITY_SDK_PATH}/include")
    message(STATUS "D3D12: Configured D3D12-specific includes for ${target}")
endfunction()

# ========= D3D12 LIBRARIES =========
function(link_d3d12_libraries target)
    target_link_libraries(${target} PUBLIC 
        D3D12MemoryAllocator
        ${DXC_LIB_DIR}
        d3d12
        dxgi
        d3dcompiler
    )
    message(STATUS "D3D12: Linked D3D12 libraries for ${target}")
endfunction()

# ========= D3D12 DEFINITIONS =========
function(setup_d3d12_definitions target)
    target_compile_definitions(${target} PRIVATE 
        IFNITY_D3D12_BACKEND
        D3D12_SDK_VERSION=${D3D12_SDK_VERSION}
        UNICODE
        _UNICODE
        ImTextureID=ImU64  # Required for D3D12 ImGui backend
    )
    message(STATUS "D3D12: Set D3D12 definitions for ${target}")
endfunction()

# ========= D3D12 CUSTOM COMMANDS =========
function(setup_d3d12_custom_commands target)
    # Copy DXC DLLs to output directory
    if(DXC_DLLS_GLOBAL)
        foreach(DXC_DLL ${DXC_DLLS_GLOBAL})
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DXC_DLL}"
                "$<TARGET_FILE_DIR:${target}>"
                COMMENT "Copying DXC DLL: ${DXC_DLL}"
            )
        endforeach()
    endif()

    # Copy Agility SDK DLLs to output directory  
    if(DX_AGILITY_DLLS_GLOBAL)
        foreach(AGILITY_DLL ${DX_AGILITY_DLLS_GLOBAL})
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory 
                "$<TARGET_FILE_DIR:${target}>/D3D12"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${AGILITY_DLL}"
                "$<TARGET_FILE_DIR:${target}>/D3D12"
                COMMENT "Copying Agility SDK DLL: ${AGILITY_DLL}"
            )
        endforeach()
    endif()
    
    message(STATUS "D3D12: Configured custom commands for ${target}")
endfunction()

# Export variables
set(D3D12_SOURCES ${D3D12_SOURCES} PARENT_SCOPE)
set(D3D12_HEADERS ${D3D12_HEADERS} PARENT_SCOPE)

list(LENGTH D3D12_SOURCES D3D12_SOURCES_COUNT)
message(STATUS "D3D12: Found ${D3D12_SOURCES_COUNT}  source files")
message(STATUS "DirectX 12 configuration complete.")