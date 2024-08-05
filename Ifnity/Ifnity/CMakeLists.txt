cmake_minimum_required(VERSION 3.8)
project(Ifnity)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DIST ${CMAKE_BINARY_DIR}/Dist)


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


#Rutas  nvml.lib 
set(NVML_LIB_PATH "${PROJECT_SOURCE_DIR}/vendor/nvml/nvml.lib")

# Add GLFW like a subdirectory to be built with the project
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/imgui)



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



message("Ruta del directorio de origen del proyecto: ${PROJECT_SOURCE_DIR}")
# Source files
file(GLOB_RECURSE SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.hpp
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Vendor Source Files 
set(GLAD_SOURCE ${PROJECT_SOURCE_DIR}/vendor/glad/src/glad.cpp)


# Add precompiled header

# Add precompiled header source files
set(PCH_SOURCES ${PROJECT_SOURCE_DIR}/src/pch.cpp)

# Add library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${PCH_SOURCES}  ${GLAD_SOURCE})




target_precompile_headers(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/pch.h)
# Check if SOURCE_FILES is empty
if(NOT SOURCE_FILES)
    message(FATAL_ERROR "No source files given to target: ${PROJECT_NAME}")
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC opengl32)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)


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


