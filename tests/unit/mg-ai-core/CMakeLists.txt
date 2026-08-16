add_executable(test_mg_ai
    test_mg_ai.c
)

target_link_libraries(test_mg_ai PRIVATE
    mg-ai-core
)

add_test(NAME mg-ai-core.unit COMMAND test_mg_ai)
