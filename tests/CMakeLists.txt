set(DOCTEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doctest)
add_library(doctest INTERFACE)
target_include_directories(doctest
    INTERFACE ${DOCTEST_INCLUDE_DIR}
    )

add_executable(imaditor-layer-tests
    layer-tests.cpp
    ../layer.cpp
    )

target_link_libraries(imaditor-layer-tests
    doctest
    )

target_compile_features(imaditor-layer-tests
    PRIVATE cxx_auto_type
    PRIVATE cxx_nullptr
    PRIVATE cxx_range_for
    PRIVATE cxx_thread_local
    )
