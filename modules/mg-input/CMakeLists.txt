add_library(mg-input STATIC
    src/mg_input.c
    src/mg_gamepad.c
)


target_include_directories(mg-input PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-input PUBLIC
    mg-pal
    mg-alloc
    mg-rx
    mg-math
)

