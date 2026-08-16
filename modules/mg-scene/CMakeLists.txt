add_library(mg-scene STATIC
    src/mg_ecs.c
    src/mg_gameplay.c
    src/mg_hierarchy.c
    src/mg_prefab.c
    src/mg_skeleton.c
    src/mg_octree.c
)






target_include_directories(mg-scene PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-scene PUBLIC
    mg-pal
    mg-alloc
    mg-math
    mg-rx
)
