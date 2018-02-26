cmake_minimum_required(VERSION 3.7)

project(imaditor)

list(APPEND CMAKE_PREFIX_PATH "$ENV{userprofile}/.cpppm/install")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_PREFIX_PATH}/cmake)

find_package(GLFW3)

add_subdirectory(tests)

if(GLFW3_FOUND)
    find_package(OpenGL REQUIRED)
    find_package(GLM REQUIRED)

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
        imgui_impl_glfw_gl3.cpp
        glad.c
        )

    target_include_directories(imgui
        PRIVATE "$ENV{userprofile}/.cpppm/install/include"
        PRIVATE include
        PRIVATE imgui
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
        PRIVATE "$ENV{userprofile}/.cpppm/install/include"
        PRIVATE include
        PRIVATE imgui
        PRIVATE nativefiledialog/src/include
        )

    target_link_libraries(imaditor
        imgui nfd glfw
        ${ZLIB_LIBRARIES}
        ${OPENGL_LIBRARIES}
        )

    target_compile_features(imaditor
        PRIVATE cxx_auto_type
        PRIVATE cxx_nullptr
        PRIVATE cxx_range_for
        PRIVATE cxx_thread_local
        )

endif(GLFW3_FOUND)
