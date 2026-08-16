add_executable(example_01_triangle
    main.c
)

target_link_libraries(example_01_triangle PRIVATE
    mg-render
)
