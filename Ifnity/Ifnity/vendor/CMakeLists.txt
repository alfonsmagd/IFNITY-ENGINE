
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