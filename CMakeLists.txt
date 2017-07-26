cmake_minimum_required(VERSION 2.8)

project(imaditor)

find_package(GLFW3)

if(GLFW3_FOUND)
    find_package(OpenGL REQUIRED)
    find_package(GLM REQUIRED)

    set(imaditor_SRCS
        main.cpp
        program.cpp
        shader.cpp
        log.cpp
        tools.cpp
        )

    set(imaditor_HDRS
        program.h
        shader.h
        log.h
        imgui_tabs.h
        tools.h
        image.h
        )

    set(actions_SRCS
        actions/baseaction.cpp
        actions/baseaction.h
        actions/floodfillaction.cpp
        actions/floodfillaction.h
        )

    set(imgui_SRCS
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui_impl_glfw_gl3.cpp
        )

    set(nfd_SRCS
        nativefiledialog/src/nfd_common.c
        nativefiledialog/src/nfd_win.cpp
        )

    add_executable(imaditor
        ${imaditor_SRCS}
        ${imaditor_HDRS}
        ${imgui_SRCS}
        ${nfd_SRCS}
        ${actions_SRCS}
        )

    target_include_directories(imaditor
        PRIVATE ${GLFW3_INCLUDE_DIR}
        PRIVATE ${GLM_INCLUDE_DIR}
        PRIVATE imgui
        PRIVATE nativefiledialog/src/include
        )

    target_link_libraries(imaditor
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
