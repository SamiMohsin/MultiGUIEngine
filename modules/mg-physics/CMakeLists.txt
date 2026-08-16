add_library(mg-physics STATIC
    src/mg_physics.c
    src/mg_physics3d.c
    src/mg_ccd.c
)



target_include_directories(mg-physics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-physics PUBLIC
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
