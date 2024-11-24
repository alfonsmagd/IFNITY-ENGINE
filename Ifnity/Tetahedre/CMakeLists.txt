cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(Tetahedre)

# C++ 17 standar 
set(CMAKE_CXX_STANDARD 17)

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")


file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")


file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/*.hpp")

# Incluye directories
include_directories(
    "${PROJECT_SOURCE_DIR}/../Ifnity/vendor/spdlog/include"
    "${PROJECT_SOURCE_DIR}/../Ifnity/src"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

#Add Ifnity dependencies 
add_dependencies(${PROJECT_NAME} Ifnity)

if(BUILD_SHARED_IFNITY)
    target_compile_definitions(${PROJECT_NAME} PRIVATE IFNITY_USE_SHARED)
    message("IFNITY_USE_SHARED CREATE  = ON ")
endif()

# Link Ifnity library dll 
target_link_libraries(${PROJECT_NAME} PUBLIC Ifnity)




# Copy Ifnity.dll inside Sandbox Proyect.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:Ifnity>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/../Shaders
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/../data
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)
