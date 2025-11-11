cmake_minimum_required(VERSION 3.8)
project(IFNITYPROYECT CXX C)

option(BUILD_SHARED_IFNITY "Build Ifnity as a shared library dll" OFF)
option(IFNITY_OPENGL_ONLY "Build Ifnity with OpenGL support only (excludes Vulkan and D3D12)" OFF)


option(IFNITY_OPENGL_API "Enable OpenGL backend" ON)
option(IFNITY_VULKAN_API "Enable Vulkan backend" OFF)
option(IFNITY_D3D12_API "Enable D3D12 backend" ON)

set(IFNITY_OPENGL_API ${IFNITY_OPENGL_API} CACHE BOOL "Enable OpenGL backend" FORCE)
set(IFNITY_VULKAN_API ${IFNITY_VULKAN_API} CACHE BOOL "Enable Vulkan backend" FORCE)
set(IFNITY_D3D12_API ${IFNITY_D3D12_API} CACHE BOOL "Enable D3D12 backend" FORCE)



set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(EXAMPLES_BASE_PATH "${CMAKE_SOURCE_DIR}/examples")

# ========= BOOTSTRAP: Download external assets =========

set(PROJECT_ROOT_DIR "${CMAKE_SOURCE_DIR}/..")
set(DATA_SCRIPT "${PROJECT_ROOT_DIR}/run_download_assets.py")
set(DATA_READY_FILE "${PROJECT_ROOT_DIR}/data/.ready")

find_package(Python3 REQUIRED COMPONENTS Interpreter)

if(NOT EXISTS ${DATA_READY_FILE})
    message(STATUS "Ifnity data assets not found. Running bootstrap script now.")
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${DATA_SCRIPT}
        WORKING_DIRECTORY ${PROJECT_ROOT_DIR}
        RESULT_VARIABLE result
    )
    if(result EQUAL 0)
        file(WRITE ${DATA_READY_FILE} "done")
    else()
        message(FATAL_ERROR "Failed to execute asset bootstrap script.")
    endif()
endif()

# ========= Build Ifnity library =========
add_subdirectory(Ifnity)


# ========= Build examples =========
# ========= Build examples =========
# Filtra ejemplos basados en la configuración
if(IFNITY_OPENGL_ONLY)
    set(EXAMPLE_DIRS
        Sandbox
        #Not Working Yet
        #Simple_GL_RenderPass
        #scene_materials  # Si es compatible con OpenGL
        # Agrega aquí otros ejemplos que funcionen solo con OpenGL
    )
else()
    set(EXAMPLE_DIRS
        Sandbox
        D3D12_SimpleTriangle
        D3D12_Scene
        SceneVk
        Triangle
        #Simple_GL_RenderPass Not Working Yet
        scene_materials
        # ... todos los ejemplos
    )
endif()

foreach(EXAMPLE ${EXAMPLE_DIRS})
    add_subdirectory("${EXAMPLES_BASE_PATH}/${EXAMPLE}" "${CMAKE_BINARY_DIR}/${EXAMPLE}")
    set_target_properties(${EXAMPLE} PROPERTIES FOLDER "Examples")
endforeach()
