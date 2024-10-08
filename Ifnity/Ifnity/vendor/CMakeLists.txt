
# imgui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui" )
set(IMGUI_FILES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imconfig.h"
    "${IMGUI_DIR}/imgui.h"
    "${IMGUI_DIR}/imgui_internal.h"
    "${IMGUI_DIR}/imstb_rectpack.h"
    "${IMGUI_DIR}/imstb_textedit.h"
    "${IMGUI_DIR}/imstb_truetype.h")

if (NOT TARGET imgui)
  add_library(imgui STATIC ${IMGUI_FILES})
  target_include_directories(imgui PUBLIC ${IMGUI_DIR})
  set_property(TARGET imgui PROPERTY FOLDER "ThirdParty")
endif()

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui" PARENT_SCOPE)

#implot
set(IMPLOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/implot" )
set(IMPLOT_FILES
  	${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot.h
    ${IMPLOT_DIR}/implot_demo.cpp
    ${IMPLOT_DIR}/implot_internal.h
    ${IMPLOT_DIR}/implot_items.cpp
)
if (NOT TARGET implot)
  add_library(implot STATIC ${IMPLOT_FILES})
  target_include_directories(implot PUBLIC ${IMPLOT_DIR})
  target_include_directories(implot PRIVATE ${IMGUI_DIR})
  set_property(TARGET implot PROPERTY FOLDER "ThirdParty")
endif()

### optional dependencies

# glm
add_library(glm INTERFACE)
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glm)
target_sources(glm INTERFACE ${GLM_DIR}/glm/glm.hpp)
target_include_directories(glm INTERFACE ${GLM_DIR})

target_compile_definitions(glm INTERFACE
    GLM_FORCE_RADIANS
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_XYZW_ONLY
)

#stb_image 
set(STB_IMAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stb_image")
add_library(stb_image INTERFACE)
target_sources(stb_image INTERFACE ${STB_IMAGE_DIR}/stb_image.h)
target_include_directories(stb_image INTERFACE ${STB_IMAGE_DIR})
set_property(TARGET stb_image PROPERTY FOLDER "ThirdParty")
