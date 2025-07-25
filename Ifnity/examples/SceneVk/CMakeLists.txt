cmake_minimum_required(VERSION 3.8)

# Nombre del proyecto
project(SceneVk)

# C++ 17 standar 
set(CMAKE_CXX_STANDARD 20)

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")


file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")


file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/*.hpp")

# Incluye directories
include_directories(
    "${PROJECT_SOURCE_DIR}/../../Ifnity/vendor/spdlog/include"
    "${PROJECT_SOURCE_DIR}/../../Ifnity/src"
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
   ${CMAKE_SOURCE_DIR}/../data
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)




# Search  .glsl, .frag, .vert, .geom, .sp, y .h dentro de src/
file(GLOB_RECURSE GLSL_SHADERS "${PROJECT_SOURCE_DIR}/src/*.glsl")
file(GLOB_RECURSE VULKAN_SHADERS "${PROJECT_SOURCE_DIR}/src/*.frag" "${PROJECT_SOURCE_DIR}/src/*.vert" "${PROJECT_SOURCE_DIR}/src/*.geom")
file(GLOB_RECURSE SPIRV_HEADERS "${PROJECT_SOURCE_DIR}/src/*.sp" "${PROJECT_SOURCE_DIR}/src/*.h")

#Build directory for shaders
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders/opengl"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders/vk"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders"
)

foreach(shader ${GLSL_SHADERS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${shader}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders/opengl"
    )
endforeach()

foreach(shader ${VULKAN_SHADERS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${shader}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders/vk"
    )
endforeach()

foreach(shader ${SPIRV_HEADERS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${shader}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders"
    )
endforeach()

# Filter and show .exe 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Executables")