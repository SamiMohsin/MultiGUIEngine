add_executable(test_mg_net
    test_mg_net.c
)

target_link_libraries(test_mg_net PRIVATE
    mg-net
)

add_test(NAME mg-net.unit COMMAND test_mg_net)
