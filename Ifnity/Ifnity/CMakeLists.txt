cmake_minimum_required(VERSION 3.8)
project(Ifnity)


#Options


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DIST ${CMAKE_BINARY_DIR}/Dist)

#DXC directories and variables 
set(DXC_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendor/dxc/include")
set(DXC_LIB_DIR "${PROJECT_SOURCE_DIR}/vendor/dxc/lib/x64/dxcompiler.lib")
# Ruta al directorio de DLLs
set(DXC_DLL_DIR "${PROJECT_SOURCE_DIR}/vendor/dxc/bin/x64")

# Copiar todas las DLLs del directorio al directorio de salida
file(GLOB DXC_DLLS "${DXC_DLL_DIR}/*.dll")

set(SPIRV_SKIP_TESTS ON CACHE BOOL "Skip SPIRV tests")
        set(SPIRV_BUILD_COMPRESSION OFF CACHE BOOL "Do not build SPIRV compressing codec")
        set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "Skip building SPIRV executable and tests along with the library")
        set(SKIP_SPIRV_TOOLS_INSTALL ON CACHE BOOL "Skip SPIRV tools installation")
        set(SPIRV_WERROR OFF CACHE BOOL "Do not enable error on warning")
		set(SPIRV_CROSS_CLI  OFF CACHE BOOL "Build the CLI binary. Requires SPIRV_CROSS_STATIC.")


# Buscar nvml.dll en el sistema
find_file(NVML_LIBRARY
    NAMES nvml.dll
    PATHS $ENV{PROGRAMFILES}/NVIDIA Corporation/NVSMI
          $ENV{SYSTEMROOT}/System32
)

if(NOT NVML_LIBRARY)
    message(STATUS "nvml.dll no se encontró en el sistema.")
else()
    message(STATUS "nvml.dll encontrado: ${NVML_LIBRARY}")
endif()


#Test if the system has vulkan,d3d11,d3d12,opengl. 
include(${PROJECT_SOURCE_DIR}/FindGraphicsAPIs.cmake)

#Rutas  nvml.lib 
set(NVML_LIB_PATH "${PROJECT_SOURCE_DIR}/vendor/nvml/nvml.lib")

# Add GLFW like a subdirectory to be built with the project
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/glfw)
#add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/glm)
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor)
# add D3D12MemoryAllocator 
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/D3D12MemoryAllocator)
# add vulkan bootstrap
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/vk-bootstrap)
#add glslang 
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/SPIRV-Cross)





# Include directories
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/vendor/spdlog/include)
# La línea siguiente ya no es necesaria porque add_subdirectory se encargará de incluir los directorios necesarios
# include_directories(${PROJECT_SOURCE_DIR}/vendor/glfw/include)
include_directories(${PROJECT_SOURCE_DIR}/vendor/glm)
#include nvml directory
include_directories(${PROJECT_SOURCE_DIR}/vendor/nvml)
#include glad directory 
include_directories(${PROJECT_SOURCE_DIR}/vendor/glad/include)
#include imgui directory
include_directories(${PROJECT_SOURCE_DIR}/vendor/imgui) 
#include implot directory
include_directories(${PROJECT_SOURCE_DIR}/vendor/implot)
#include stb_image 
include_directories(${PROJECT_SOURCE_DIR}/vendor/stb_image)
#include d3d12mmemoryallocator and vkmemoryallocator
include_directories(${PROJECT_SOURCE_DIR}/vendor/D3D12MemoryAllocator/include)
# Include Vulkan directories
include_directories(${Vulkan_INCLUDE_DIRS})
# Include VK-Bootstrap
include_directories(${PROJECT_SOURCE_DIR}/vendor/vk-bootstrap)
#Include DXC
include_directories(${DXC_INCLUDE_DIR})




message("Ruta del directorio de origen del proyecto: ${PROJECT_SOURCE_DIR}")
# Source files
file(GLOB_RECURSE SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.hpp
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.c
)

# Vendor Source Files 
set(GLAD_SOURCE ${PROJECT_SOURCE_DIR}/vendor/glad/src/glad.cpp)


# Add precompiled header

# Add precompiled header source files
set(PCH_SOURCES ${PROJECT_SOURCE_DIR}/src/pch.cpp)

# Add library
if(BUILD_SHARED_IFNITY)
	add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${PCH_SOURCES}  ${GLAD_SOURCE})
    target_compile_definitions(${PROJECT_NAME} PRIVATE IFNITY_PLATFORM_WINDOWS BUILD_SHARED_IFNITY)
     message("IFNITY LIKE DLL BUILD SHARED IFNITY = ON ")
else()
	add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${PCH_SOURCES}  ${GLAD_SOURCE})
endif()


target_precompile_headers(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/pch.h)

# Check if SOURCE_FILES is empty
if(NOT SOURCE_FILES)
    message(FATAL_ERROR "No source files given to target: ${PROJECT_NAME}")
endif()



# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC opengl32)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC implot)
target_link_libraries(${PROJECT_NAME} INTERFACE  glm)
target_link_libraries(${PROJECT_NAME} INTERFACE  stb_image)
target_link_libraries(${PROJECT_NAME} PUBLIC D3D12MemoryAllocator)
# Link Vulkan libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
# Link VK-Bootstrap
target_link_libraries(${PROJECT_NAME} PUBLIC vk-bootstrap::vk-bootstrap)
target_link_libraries(${PROJECT_NAME} PUBLIC spirv-cross-core spirv-cross-c)
#Link dxc
target_link_libraries(${PROJECT_NAME} PUBLIC ${DXC_LIB_DIR})

# Include Target Directories
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/vma)
# Include Target DXC Directories
target_include_directories(${PROJECT_NAME} PUBLIC ${DXC_INCLUDE_DIR})


if(NVML_LIBRARY)
    message(STATUS "Enlazando nvml.lib ya que nvml.dll .")
    target_compile_definitions(${PROJECT_NAME} PUBLIC NVP_SUPPORTS_NVML)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${NVML_LIB_PATH})
else()
    # nvml.dll no fue encontrado
    message(STATUS "nvml.dll no fue encontrado. nvml.lib no será enlazado.")
endif()


  
# Preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE IFNITY_PLATFORM_WINDOWS IFNITY_EXPORTS_DLL)

# Configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist")

# Antes de definir las propiedades del target, establece las banderas del enlazador para la configuración Dist
set(CMAKE_SHARED_LINKER_FLAGS_DIST "-O2" CACHE STRING "Flags used by the linker during Dist builds.")



# Flags for different configurations
set_target_properties(${PROJECT_NAME} PROPERTIES
    COMPILE_FLAGS_DEBUG "-D IFNITY_DEBUG"
    COMPILE_FLAGS_RELEASE "-D IFNITY_RELEASE -O2"
    COMPILE_FLAGS_DIST "-D IFNITY_DIST -O2"
    LINK_FLAGS_DIST "${CMAKE_SHARED_LINKER_FLAGS_DIST}"
)

# Copiar nvml.lib al directorio de enlace
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/vendor/nvml/nvml.lib
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/vendor/nvml/nvml.lib
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/vendor/nvml/nvml.lib
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DIST}
)

foreach(DLL_PATH IN LISTS DXC_DLLS)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_PATH}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
    )
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_PATH}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
    )
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_PATH}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DIST}
    )
endforeach()


