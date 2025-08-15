# Core Engine Configuration - Código compartido por todos los backends
message(STATUS "Configuring Core Engine components...")

# ========= CORE ENGINE SOURCES =========
set(CORE_ENGINE_SOURCES)
set(CORE_ENGINE_HEADERS)

# Archivos del núcleo del engine (siempre incluidos)
file(GLOB_RECURSE IFNITY_CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Ifnity/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Ifnity/*.c"
)

file(GLOB_RECURSE IFNITY_CORE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Ifnity/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Ifnity/*.hpp"
)

# Graphics interfaces y código común (siempre incluido)
file(GLOB_RECURSE GRAPHICS_CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/*.c"
)

# Graphics interfaces y código común (siempre incluido)
file(GLOB_RECURSE BENCHMARK_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/BenchMark/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/BenchMark/*.c"
)

file(GLOB_RECURSE BENCHMARK_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/BenchMark/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/BenchMark/*.hpp"
)

# Graphics interfaces y Renderers
file(GLOB_RECURSE RENDERERS_CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Renderers/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Renderers/*.c"
)

# Headers de Graphics interfaces y Renderers
file(GLOB_RECURSE RENDERERS_CORE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Renderers/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Renderers/*.hpp"
)

file(GLOB_RECURSE GRAPHICS_CORE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/*.hpp"
)

# Código común de utilidades
file(GLOB_RECURSE UTILS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/*.c"
)

file(GLOB_RECURSE UTILS_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/*.hpp"
)

# Shader Builders 
file(GLOB_RECURSE SHADERBUILDER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderBuilding/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderBuilding/*.c"
)

file(GLOB_RECURSE SHADERBUILDER_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderBuilding/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderBuilding/*.hpp"
)


file(GLOB ROOT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(GLOB ROOT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)



# Combinar archivos del core
list(APPEND CORE_ENGINE_SOURCES 
    ${IFNITY_CORE_SOURCES}
    ${GRAPHICS_CORE_SOURCES}
    ${UTILS_SOURCES}
    ${RENDERERS_CORE_SOURCES}
    ${SHADERBUILDER_SOURCES}
    ${BENCHMARK_SOURCES}
    ${ROOT_SOURCES}
)



list(APPEND CORE_ENGINE_HEADERS
    ${IFNITY_CORE_HEADERS}
    ${GRAPHICS_CORE_HEADERS}
    ${UTILS_HEADERS}
    ${RENDERERS_CORE_HEADERS}
    ${SHADERBUILDER_HEADERS}
    ${BENCHMARK_HEADERS}
    ${ROOT_HEADERS}


)

# ========= SHARED VENDOR DEPENDENCIES =========
# Estas son dependencias que SIEMPRE se incluyen independientemente del backend
set(SHARED_VENDOR_SOURCES
    # GLSL lang (para shader compilation - usado por todos)
    ${PROJECT_SOURCE_DIR}/vendor/glslang/glslang/ResourceLimits/ResourceLimits.cpp
    ${PROJECT_SOURCE_DIR}/vendor/glslang/glslang/ResourceLimits/resource_limits_c.cpp
)

list(APPEND CORE_ENGINE_SOURCES ${SHARED_VENDOR_SOURCES})

# ========= CORE INCLUDES =========
function(setup_core_includes target)
    target_include_directories(${target} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/implot"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/rapidjson/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp/include"
    )
    message(STATUS "Core Engine: Configured core includes for ${target}")
endfunction()

# ========= SHARED LIBRARIES =========
function(link_core_libraries target)
    # Estas librerías SIEMPRE se enlazan
    target_link_libraries(${target} PUBLIC 
        glfw
        imgui
        implot
        glslang
        assimp
    )
    
    target_link_libraries(${target} INTERFACE 
        glm
        stb_image
    )
    
    message(STATUS "Core Engine: Linked core libraries for ${target}")
endfunction()

# ========= CORE DEFINITIONS =========
function(setup_core_definitions target)
    target_compile_definitions(${target} PRIVATE 
        IFNITY_PLATFORM_WINDOWS 
        #IFNITY_EXPORTS_DLL
    )
    message(STATUS "Core Engine: Set core definitions for ${target}")
endfunction()

# Export variables
set(CORE_ENGINE_SOURCES ${CORE_ENGINE_SOURCES} PARENT_SCOPE)
set(CORE_ENGINE_HEADERS ${CORE_ENGINE_HEADERS} PARENT_SCOPE)

# Get the length of the list correctly
list(LENGTH CORE_ENGINE_SOURCES CORE_ENGINE_SOURCES_COUNT)
message(STATUS "Core Engine: Found ${CORE_ENGINE_SOURCES_COUNT} source files")
message(STATUS "Core Engine configuration complete.")

