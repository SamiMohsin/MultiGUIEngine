add_executable(example_15_m3_art_and_theme_showcase
    main.c
)

target_link_libraries(example_15_m3_art_and_theme_showcase PRIVATE
    mg-ui
    mg-render
    mg-pal
    mg-alloc
    mg-rx
)
