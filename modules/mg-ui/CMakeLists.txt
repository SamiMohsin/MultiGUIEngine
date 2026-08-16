add_library(mg-ui STATIC
    src/mg_ui.c
)

target_include_directories(mg-ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-ui PUBLIC
    mg-render
    mg-pal
    mg-alloc
    mg-rx
)
