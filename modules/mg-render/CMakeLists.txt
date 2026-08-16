add_library(mg-render STATIC
    src/mg_rhi.c
    src/mg_batch2d.c
    src/backend_software/mg_backend_software.c
    src/backend_vulkan/mg_backend_vulkan.c
    src/mg_postprocess.c
    src/mg_font.c
)



target_include_directories(mg-render PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-render PUBLIC
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
