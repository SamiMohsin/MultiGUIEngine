add_executable(example_37_rich_markdown_text_layout
    main.c
)

target_link_libraries(example_37_rich_markdown_text_layout PRIVATE
    mg-ui
    mg-render
    mg-pal
    mg-alloc
    mg-math
    mg-rx
    m
)
