add_executable(example_07_material_gallery
    main.c
)

target_link_libraries(example_07_material_gallery PRIVATE
    mg-ui
    mg-render
    mg-pal
    mg-alloc
    mg-rx
)
