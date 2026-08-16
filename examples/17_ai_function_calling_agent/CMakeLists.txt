add_executable(example_17_ai_function_calling_agent
    main.c
)

target_link_libraries(example_17_ai_function_calling_agent PRIVATE
    mg-ai-core
    mg-pal
    mg-alloc
    mg-rx
)
