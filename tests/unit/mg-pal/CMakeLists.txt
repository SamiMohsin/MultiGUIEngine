add_executable(test_mg_pal test_mg_pal.c)
target_link_libraries(test_mg_pal PRIVATE mg-pal)
add_test(NAME mg-pal.version_string COMMAND test_mg_pal)
