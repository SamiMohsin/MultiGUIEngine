add_executable(test_mg_audio
    test_mg_audio.c
)

target_link_libraries(test_mg_audio PRIVATE
    mg-audio
)

add_test(NAME mg-audio.unit COMMAND test_mg_audio)
