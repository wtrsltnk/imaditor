cmake_minimum_required(VERSION 3.7)

include(cmake/CPM.cmake)
include(cmake/Dependencies.cmake)

project(imaditor)

add_subdirectory(tests)

find_package(OpenGL REQUIRED)

add_library(nfd
    nativefiledialog/src/nfd_common.c
    nativefiledialog/src/nfd_win.cpp
)

target_include_directories(nfd
    PRIVATE nativefiledialog/src/include
)

add_library(imgui
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    glad.c
)

target_include_directories(imgui
    PRIVATE include
    PRIVATE imgui
)

target_link_libraries(imgui
    PRIVATE glfw
)

target_compile_definitions(imgui
    PUBLIC IMGUI_USE_WCHAR32
)

add_executable(imaditor
    actions/baseaction.cpp
    actions/baseaction.h
    actions/floodfillaction.cpp
    actions/floodfillaction.h
    actions/brushaction.cpp
    actions/brushaction.h
    actions/dropperaction.cpp
    actions/dropperaction.h
    actions/eraseaction.cpp
    actions/eraseaction.h
    main.cpp
    program.cpp
    program.h
    shader.cpp
    shader.h
    log.cpp
    log.h
    tools.cpp
    tools.h
    imgui_tabs.cpp
    imgui_tabs.h
    image.cpp
    image.h
    images.cpp
    images.h
    layer.cpp
    layer.h
    brushes.cpp
    brushes.h
    glarraybuffer.cpp
    glarraybuffer.h
    glprogram.cpp
    glprogram.h
    state.h
)

target_include_directories(imaditor
    PRIVATE include
    PRIVATE imgui
    PRIVATE nativefiledialog/src/include
)

target_link_libraries(imaditor
    imgui
    nfd
    glfw
    glm
    ${OPENGL_LIBRARIES}
)

target_compile_features(imaditor
    PRIVATE cxx_auto_type
    PRIVATE cxx_nullptr
    PRIVATE cxx_range_for
    PRIVATE cxx_thread_local
)

