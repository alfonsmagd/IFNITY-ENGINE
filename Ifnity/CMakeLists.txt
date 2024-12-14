cmake_minimum_required(VERSION 3.8)
project(IFNITYPROYECT)

# Define la opción y propágala a los subdirectorios
option(BUILD_SHARED_IFNITY "Build Ifnity as a shared library dll " OFF)
# Asegurar modulos cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Añade el subdirectorio que contiene el proyecto Ifnity
add_subdirectory(Ifnity)

# Añade el subdirectorio que contiene el proyecto Sandbox Y EJEMPLOS 
add_subdirectory(Sandbox)
add_subdirectory(GPU_MONITOR)
add_subdirectory(ShadersPipeLine)
add_subdirectory(Simple_Texture)
add_subdirectory(Tetahedre)
add_subdirectory(Skybox_Camera)
add_subdirectory(MeshGeometrical_Draw)
add_subdirectory(LargeScene)

#add_subdirectory(Source_VTXP_HLSL)
#add_subdirectory(CubeMap_Texture)

# El dll que genera Ifnity se copiará automáticamente a la carpeta de salida de Sandbox

