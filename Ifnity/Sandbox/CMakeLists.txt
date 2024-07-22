cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(Sandbox)

# Establece C++ como el lenguaje del proyecto
set(CMAKE_CXX_STANDARD 17)

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

# Directorios de los archivos fuente
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Directorios de los archivos de cabecera
file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/*.h")

# Incluye directorios
include_directories(
    "${PROJECT_SOURCE_DIR}/../Ifnity/vendor/spdlog/include"
    "${PROJECT_SOURCE_DIR}/../Ifnity/src"
    "${PROJECT_SOURCE_DIR}/../Ifnity/vendor/nvml"
)

# Especifica el ejecutable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Asegúrate de que la ruta al proyecto Ifnity esté correctamente especificada
# y de que Ifnity se construya antes de Sandbox
add_dependencies(${PROJECT_NAME} Ifnity)

# Agrega el directorio que contiene el .lib a las rutas de enlace
link_directories("${CMAKE_BINARY_DIR}/Debug")
link_directories("${CMAKE_BINARY_DIR}/Release")
link_directories("${CMAKE_BINARY_DIR}/Dist")

# Vincula la biblioteca Ifnity al proyecto Sandbox
target_link_libraries(${PROJECT_NAME} PUBLIC Ifnity)



add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/../Ifnity/vendor/nvml/nvml.lib"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Asegúrate de copiar la DLL al directorio de salida para que pueda ser encontrada en tiempo de ejecución
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:Ifnity>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/../Ifnity/vendor/nvml/nvml.lib"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)