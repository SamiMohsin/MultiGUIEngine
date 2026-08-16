add_executable(mg-editor
    main.c
)

target_link_libraries(mg-editor PRIVATE
    mg-ui
    mg-render
    mg-scene
    mg-input
    mg-ai-core
    mg-pal
    mg-alloc
    mg-rx
)
