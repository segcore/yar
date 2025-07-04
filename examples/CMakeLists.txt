add_executable(simple simple.c)
target_link_libraries(simple PRIVATE yar)

add_executable(string_builder string_builder.c)
target_link_libraries(string_builder PRIVATE yar)

add_executable(large_structs large_structs.c)
target_link_libraries(large_structs PRIVATE yar)
