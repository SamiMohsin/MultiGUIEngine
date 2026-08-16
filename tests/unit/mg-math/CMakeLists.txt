add_executable(test_mg_math
    test_mg_math.c
)

target_link_libraries(test_mg_math PRIVATE
    mg-math
)

add_test(NAME mg-math.unit COMMAND test_mg_math)
