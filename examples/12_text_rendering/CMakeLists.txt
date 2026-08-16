add_executable(example_12_text_rendering
    main.c
)

target_link_libraries(example_12_text_rendering PRIVATE
    mg-render
    mg-pal
    mg-alloc
    mg-rx
)
