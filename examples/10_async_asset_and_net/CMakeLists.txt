add_executable(example_10_async_asset_and_net
    main.c
)

target_link_libraries(example_10_async_asset_and_net PRIVATE
    mg-asset
    mg-net
    mg-render
    mg-pal
    mg-alloc
    mg-rx
)
