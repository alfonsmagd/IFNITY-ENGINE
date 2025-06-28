cmake_minimum_required(VERSION 3.8)
project(IFNITYPROYECT CXX C)

option(BUILD_SHARED_IFNITY "Build Ifnity as a shared library dll" OFF)

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
set(EXAMPLE_DIRS
    Sandbox
    #ShadersPipeLine
    D3D12_SimpleTriangle
    #D3Texture
    #D3D12_Cube_MVP
    #D3D12_Assimp
    D3D12_Scene
    #d3d12_test_scene
    #d3d12_SceneDebug
    #Simple_Texture
    scene_materials
    #Geometry_Mesh
    #glslang_vk_test
    #bufferVulkan
    #Texture_Sample
    #camera_mesh_vk
    SceneVk
)

foreach(EXAMPLE ${EXAMPLE_DIRS})
    add_subdirectory("${EXAMPLES_BASE_PATH}/${EXAMPLE}" "${CMAKE_BINARY_DIR}/${EXAMPLE}")
    set_target_properties(${EXAMPLE} PROPERTIES FOLDER "Examples")
endforeach()

unset(CMAKE_EXE_LINKER_FLAGS_DIST CACHE)
set(CMAKE_EXE_LINKER_FLAGS_DIST "")
