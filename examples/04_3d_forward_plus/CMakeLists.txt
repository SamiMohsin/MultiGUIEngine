add_executable(example_04_3d_forward_plus
    main.c
)

target_link_libraries(example_04_3d_forward_plus PRIVATE
    mg-render
    mg-math
)
