add_executable(test_mg_rx
    test_mg_rx.c
)

target_link_libraries(test_mg_rx PRIVATE
    mg-rx
)

add_test(NAME mg-rx.unit COMMAND test_mg_rx)
