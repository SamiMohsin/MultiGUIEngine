add_executable(example_23_asset_bundles_and_hot_reload
    main.c
)

target_link_libraries(example_23_asset_bundles_and_hot_reload PRIVATE
    mg-asset
    mg-pal
    mg-alloc
    mg-rx
)
