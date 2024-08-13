cmake_minimum_required(VERSION 3.8)
project(IFNITYPROYECT)

# Asegurar modulos cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Añade el subdirectorio que contiene el proyecto Ifnity
add_subdirectory(Ifnity)

# Añade el subdirectorio que contiene el proyecto Sandbox
add_subdirectory(Sandbox)


# El dll que genera Ifnity se copiará automáticamente a la carpeta de salida de Sandbox

