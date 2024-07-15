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

# Agregar GLFW como un subdirectorio para que sea construido con el proyecto
add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/glfw)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/vendor/spdlog/include)
# La línea siguiente ya no es necesaria porque add_subdirectory se encargará de incluir los directorios necesarios
# include_directories(${PROJECT_SOURCE_DIR}/vendor/glfw/include)
include_directories(${PROJECT_SOURCE_DIR}/vendor/glm)


message("Ruta del directorio de origen del proyecto: ${PROJECT_SOURCE_DIR}")
# Source files
file(GLOB_RECURSE SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)
# Add precompiled header

# Add precompiled header source files
set(PCH_SOURCES ${PROJECT_SOURCE_DIR}/src/pch.cpp)

# Add library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${PCH_SOURCES} "src/Ifnity/Window.cpp" "src/Ifnity/Event/EventSystem.h" "src/Ifnity/Event/EventMacro.h")



target_precompile_headers(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/pch.h)
# Check if SOURCE_FILES is empty
if(NOT SOURCE_FILES)
    message(FATAL_ERROR "No source files given to target: ${PROJECT_NAME}")
endif()

# Link libraries
# Ahora vinculamos con el target glfw en lugar de glfw3
target_link_libraries(${PROJECT_NAME} glfw opengl32)

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

# Opcional: establece propiedades adicionales para la biblioteca
set_target_properties(Ifnity PROPERTIES OUTPUT_NAME "ifnity")


