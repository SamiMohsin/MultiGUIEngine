add_executable(test_mg_input
    test_mg_input.c
)

target_link_libraries(test_mg_input PRIVATE
    mg-input
)

add_test(NAME mg-input.unit COMMAND test_mg_input)
