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

# El dll que genera Ifnity se copiará automáticamente a la carpeta de salida de Sandbox
# Esto es para que no tengas que copiar manualmente el archivo dll cada vez que lo generes
