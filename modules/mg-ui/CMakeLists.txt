add_library(mg-ui STATIC
    src/mg_ui.c
    src/mg_material3.c
    src/mg_layout.c
    src/mg_chart.c
    src/mg_nodegraph.c
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
