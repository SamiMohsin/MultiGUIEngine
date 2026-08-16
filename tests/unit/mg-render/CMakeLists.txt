add_executable(test_mg_render
    test_mg_render.c
)

target_link_libraries(test_mg_render PRIVATE
    mg-render
)

add_test(NAME mg-render.unit COMMAND test_mg_render)
