add_library(mg-asset STATIC
    src/mg_asset.c
    src/mg_bundle.c
)


target_include_directories(mg-asset PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-asset PUBLIC
    mg-pal
    mg-alloc
    mg-rx
)
