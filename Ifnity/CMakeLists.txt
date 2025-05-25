cmake_minimum_required(VERSION 3.8)
project(IFNITYPROYECT CXX C)

# Define la opción y propágala a los subdirectorios
option(BUILD_SHARED_IFNITY "Build Ifnity as a shared library dll " OFF)
# Asegurar modulos cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(EXAMPLES_BASE_PATH "${CMAKE_SOURCE_DIR}/examples")

# Añade el subdirectorio que contiene el proyecto Ifnity
add_subdirectory(Ifnity)


set(EXAMPLE_DIRS
    Sandbox
    ShadersPipeLine
    D3D12_SimpleTriangle
    D3Texture
	D3D12_Cube_MVP
	D3D12_Assimp
    #Simple_Texture
    #scene_materials
    #Geometry_Mesh
	#glslang_vk_test
	#bufferVulkan
   #Texture_Sample
   #camera_mesh_vk
   #SceneVk
)

# Itera sobre la lista y añade cada subdirectorio
foreach(EXAMPLE ${EXAMPLE_DIRS})
    add_subdirectory("${EXAMPLES_BASE_PATH}/${EXAMPLE}" "${CMAKE_BINARY_DIR}/${EXAMPLE}")
    set_target_properties(${EXAMPLE} PROPERTIES FOLDER "Examples") #to configure in visual studio more organized
endforeach()
# El dll que genera Ifnity se copiará automáticamente a la carpeta de salida de Sandbox

