add_executable(test_mg_asset
    test_mg_asset.c
)

target_link_libraries(test_mg_asset PRIVATE
    mg-asset
)

add_test(NAME mg-asset.unit COMMAND test_mg_asset)
