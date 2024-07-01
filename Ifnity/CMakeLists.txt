cmake_minimum_required(VERSION 3.8)
project(IFNITYPROYECT)

# Asegúrate de que CMake pueda encontrar los scripts adicionales de CMake que podrías necesitar
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Añade el subdirectorio que contiene el proyecto Ifnity
# Asume que Ifnity está en un subdirectorio llamado Ifnity en la raíz de tu proyecto
add_subdirectory(Ifnity)

# Añade el subdirectorio que contiene el proyecto Sandbox
# Asume que Sandbox está en un subdirectorio llamado Sandbox en la raíz de tu proyecto
add_subdirectory(Sandbox)