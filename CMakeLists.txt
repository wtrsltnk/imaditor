cmake_minimum_required(VERSION 2.8)

project(imaditor)

find_package(GLFW3)

add_subdirectory(tests)

add_library(nfd
    nativefiledialog/src/nfd_common.c
    nativefiledialog/src/nfd_win.cpp
    )

target_include_directories(nfd
    PRIVATE nativefiledialog/src/include
)

if(GLFW3_FOUND)
    find_package(OpenGL REQUIRED)
    find_package(GLM REQUIRED)

    add_library(imgui
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui_impl_glfw_gl3.cpp
        )

    target_include_directories(imgui
        PRIVATE imgui
        PRIVATE ${GLFW3_INCLUDE_DIR}
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
        PRIVATE ${GLFW3_INCLUDE_DIR}
        PRIVATE ${GLM_INCLUDE_DIR}
        PRIVATE imgui
        PRIVATE nativefiledialog/src/include
        )

    target_link_libraries(imaditor
        imgui nfd
        ${ZLIB_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${GLFW3_LIBRARY}
        )

    target_compile_features(imaditor
        PRIVATE cxx_auto_type
        PRIVATE cxx_nullptr
        PRIVATE cxx_range_for
        PRIVATE cxx_thread_local
        )

endif(GLFW3_FOUND)
