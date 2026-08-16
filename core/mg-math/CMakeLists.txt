add_library(mg-math STATIC
    src/mg_vec2.c
    src/mg_vec3.c
    src/mg_vec4.c
    src/mg_mat4.c
    src/mg_quat.c
)

target_include_directories(mg-math PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mg-math PUBLIC
    mg-pal
    m
)
