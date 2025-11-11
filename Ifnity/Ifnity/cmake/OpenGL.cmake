# OpenGL Configuration - Platform-specific OpenGL backend
message(STATUS "Configuring OpenGL backend components...")

# ========= OPENGL BACKEND SOURCES =========
set(OPENGL_SOURCES)
set(OPENGL_HEADERS)

# Platform-specific OpenGL files
file(GLOB_RECURSE OPENGL_PLATFORM_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/gl_backend.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/DeviceOpengl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/OpenGL/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/OpenGL/*.c"
)

file(GLOB_RECURSE OPENGL_PLATFORM_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/gl_backend.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/gl_constants.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows/DeviceOpengl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/OpenGL/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/OpenGL/*.hpp"
)

# OpenGL ImGui rendering backend
file(GLOB_RECURSE OPENGL_IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/ImguiRender/ImguiOpenglRender.cpp"
)

file(GLOB_RECURSE OPENGL_IMGUI_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/ImguiRender/ImguiOpenglRender.h"
)

# Combine OpenGL files
list(APPEND OPENGL_SOURCES 
    ${OPENGL_PLATFORM_SOURCES}
    ${OPENGL_IMGUI_SOURCES}
)

list(APPEND OPENGL_HEADERS
    ${OPENGL_PLATFORM_HEADERS}
    ${OPENGL_IMGUI_HEADERS}
)

# ========= OPENGL VENDOR DEPENDENCIES =========
# OpenGL-specific vendor dependencies
set(OPENGL_VENDOR_SOURCES
    # GLAD OpenGL loader
    ${PROJECT_SOURCE_DIR}/vendor/glad/src/glad.c
)

list(APPEND OPENGL_SOURCES ${OPENGL_VENDOR_SOURCES})

# ========= OPENGL INCLUDES =========
function(setup_opengl_includes target)
    target_include_directories(${target} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include"
    )
    message(STATUS "OpenGL: Configured OpenGL-specific includes for ${target}")
endfunction()

# ========= OPENGL LIBRARIES =========
function(link_opengl_libraries target)
    target_link_libraries(${target} PUBLIC 
        opengl32  # Windows OpenGL library
    )
    message(STATUS "OpenGL: Linked OpenGL libraries for ${target}")
endfunction()

# ========= OPENGL DEFINITIONS =========
function(setup_opengl_definitions target)
    target_compile_definitions(${target} PRIVATE 
        IFNITY_OPENGL_BACKEND
        IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    )
    message(STATUS "OpenGL: Set OpenGL definitions for ${target}")
endfunction()

# Export variables
set(OPENGL_SOURCES ${OPENGL_SOURCES} PARENT_SCOPE)
set(OPENGL_HEADERS ${OPENGL_HEADERS} PARENT_SCOPE)

# Get the length of the list correctly
list(LENGTH OPENGL_SOURCES OPENGL_SOURCES_COUNT)
message(STATUS "OpenGL: Found ${OPENGL_SOURCES_COUNT} source files")
message(STATUS "OpenGL configuration complete.")


# Print files by category
message(STATUS "=== OPENGL FILES BY CATEGORY ===")

message(STATUS "--- OPENGL PLATFORM SOURCES ---")
foreach(FILE ${OPENGL_PLATFORM_SOURCES})
    message(STATUS "  ${FILE}")
endforeach()

message(STATUS "--- OPENGL PLATFORM HEADERS ---")
foreach(FILE ${OPENGL_PLATFORM_HEADERS})
    message(STATUS "  ${FILE}")
endforeach()

message(STATUS "--- OPENGL IMGUI SOURCES ---")
foreach(FILE ${OPENGL_IMGUI_SOURCES})
    message(STATUS "  ${FILE}")
endforeach()

message(STATUS "--- OPENGL IMGUI HEADERS ---")
foreach(FILE ${OPENGL_IMGUI_HEADERS})
    message(STATUS "  ${FILE}")
endforeach()

message(STATUS "--- OPENGL VENDOR SOURCES ---")
foreach(FILE ${OPENGL_VENDOR_SOURCES})
    message(STATUS "  ${FILE}")
endforeach()

message(STATUS "OpenGL configuration complete.")