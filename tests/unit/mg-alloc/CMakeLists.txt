add_executable(test_mg_alloc
    test_mg_alloc.c
)

target_link_libraries(test_mg_alloc PRIVATE
    mg-alloc
)

add_test(NAME mg-alloc.unit COMMAND test_mg_alloc)
