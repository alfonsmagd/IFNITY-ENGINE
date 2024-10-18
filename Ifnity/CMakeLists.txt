cmake_minimum_required(VERSION 3.8)
project(IFNITYPROYECT)

# Define la opción y propágala a los subdirectorios
option(BUILD_SHARED_IFNITY "Build Ifnity as a shared library dll " ON)
# Asegurar modulos cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Añade el subdirectorio que contiene el proyecto Ifnity
add_subdirectory(Ifnity)

# Añade el subdirectorio que contiene el proyecto Sandbox Y EJEMPLOS 
add_subdirectory(Sandbox)
add_subdirectory(GPU_MONITOR)
add_subdirectory(ShadersPipeLine)

# El dll que genera Ifnity se copiará automáticamente a la carpeta de salida de Sandbox

