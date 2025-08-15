# Vulkan Configuration - Platform-specific Vulkan backend
message(STATUS "Configuring Vulkan backend components...")

# ========= VULKAN BACKEND SOURCES =========
set(VULKAN_SOURCES)
set(VULKAN_HEADERS)

# Platform-specific Vulkan files
file(GLOB_RECURSE VULKAN_PLATFORM_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/vk_backend.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/vk_constants.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/UtilsVulkan.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/DeviceVulkan.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Vulkan/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Vulkan/*.c"
)

file(GLOB_RECURSE VULKAN_PLATFORM_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/vk_backend.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/DeviceVulkan.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/vk_constants.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/UtilsVulkan.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Vulkan/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Vulkan/*.hpp"
)

# Vulkan ImGui rendering backend
file(GLOB_RECURSE VULKAN_IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/ImguiRender/ImguiVulkanRender.cpp"
)

file(GLOB_RECURSE VULKAN_IMGUI_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/ImguiRender/ImguiVulkanRender.h"
)

# Combine Vulkan files
list(APPEND VULKAN_SOURCES 
    ${VULKAN_PLATFORM_SOURCES}
    ${VULKAN_IMGUI_SOURCES}
)

list(APPEND VULKAN_HEADERS
    ${VULKAN_PLATFORM_HEADERS}
    ${VULKAN_IMGUI_HEADERS}
)

# ========= VULKAN VENDOR DEPENDENCIES =========
# Vulkan-specific vendor dependencies
set(VULKAN_VENDOR_SOURCES
    # SPIRV-Cross for shader reflection
    # VK-Bootstrap for Vulkan initialization
    # These are handled by add_subdirectory in main CMakeLists.txt
)

list(APPEND VULKAN_SOURCES ${VULKAN_VENDOR_SOURCES})

# ========= VULKAN INCLUDES =========
function(setup_vulkan_includes target)
    target_include_directories(${target} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/vma"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/vk-bootstrap"
        ${Vulkan_INCLUDE_DIRS}
    )
    message(STATUS "Vulkan: Configured Vulkan-specific includes for ${target}")
endfunction()

# ========= VULKAN LIBRARIES =========
function(link_vulkan_libraries target)
    target_link_libraries(${target} PUBLIC 
        ${Vulkan_LIBRARIES}
        vk-bootstrap::vk-bootstrap
        spirv-cross-core 
        spirv-cross-c
    )
    message(STATUS "Vulkan: Linked Vulkan libraries for ${target}")
endfunction()

# ========= VULKAN DEFINITIONS =========
function(setup_vulkan_definitions target)
    target_compile_definitions(${target} PRIVATE 
        IFNITY_VULKAN_BACKEND
        VK_USE_PLATFORM_WIN32_KHR
        IMGUI_IMPL_VULKAN_NO_PROTOTYPES
    )
    message(STATUS "Vulkan: Set Vulkan definitions for ${target}")
endfunction()

# Export variables
set(VULKAN_SOURCES ${VULKAN_SOURCES} PARENT_SCOPE)
set(VULKAN_HEADERS ${VULKAN_HEADERS} PARENT_SCOPE)

list(LENGTH VULKAN_SOURCES VULKAN_SOURCES_COUNT)
message(STATUS "Vulkan: Found ${VULKAN_SOURCES_COUNT} source files")
message(STATUS "Vulkan configuration complete.")