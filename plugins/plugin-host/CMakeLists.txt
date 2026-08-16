add_library(plugin-host STATIC
    src/mg_plugin.c
)

target_include_directories(plugin-host PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(plugin-host PUBLIC
    mg-scene
    mg-pal
    mg-alloc
    mg-rx
)
