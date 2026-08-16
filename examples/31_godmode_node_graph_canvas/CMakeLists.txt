add_executable(example_31_godmode_node_graph_canvas
    main.c
)

target_link_libraries(example_31_godmode_node_graph_canvas PRIVATE
    mg-ui
    mg-render
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
