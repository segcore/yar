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

# Can be disable if C++ compiler is not available
option(TEST_YAR_CPP "Build the C++ tests for Yar" ON)
if (TEST_YAR_CPP)
    enable_language(CXX)
    test(zz_c++ zz_c++.cpp)
    target_link_libraries(zz_c++ PRIVATE yar_impl)
endif()
