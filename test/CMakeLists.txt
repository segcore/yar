function(test name)
    add_executable(${name} ${ARGV})
    target_link_libraries(${name} PRIVATE yar)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

test(append append.c)
test(append_many append_many.c)
test(reserve reserve.c)
test(insert insert.c)
test(remove remove.c)

# Ensure that the 'assert' macro is working as intended
add_executable(asserts asserts.c)
target_link_libraries(asserts PRIVATE yar)
add_test(NAME asserts COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:asserts>)
set_property(TEST asserts PROPERTY WILL_FAIL TRUE)
