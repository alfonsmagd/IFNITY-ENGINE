cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(Sandbox)

# Establece C++ como el lenguaje del proyecto
set(CMAKE_CXX_STANDARD 17)

message(PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR})
# Directorios de los archivos fuente
file(GLOB_RECURSE SOURCES
     "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# Directorios de los archivos de cabecera
file(GLOB_RECURSE HEADERS
     "${PROJECT_SOURCE_DIR}/src/*.h"
)

# Incluye directorios
include_directories(
    "${PROJECT_SOURCE_DIR}/../Ifnity/vendor/spdlog/include"
    "${PROJECT_SOURCE_DIR}/../Ifnity/src"
)

# Especifica el ejecutable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Asegúrate de que la ruta al proyecto Ifnity esté correctamente especificada
# y de que Ifnity se construya antes de Sandbox
add_dependencies(${PROJECT_NAME} Ifnity)

# Enlaza la biblioteca Ifnity al proyecto Sandbox
# Asegúrate de que el directorio de Ifnity.dll esté en tu PATH o especifica su ruta aquí
target_link_libraries(${PROJECT_NAME}
    Ifnity
)

# Configura los directorios de salida
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:Ifnity>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)