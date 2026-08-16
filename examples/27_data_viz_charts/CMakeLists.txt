add_executable(example_27_data_viz_charts
    main.c
)

target_link_libraries(example_27_data_viz_charts PRIVATE
    mg-ui
    mg-render
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
