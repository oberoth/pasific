link_libraries(${GTEST_BOTH_LIBRARIES})
add_executable(gtest gtest.cpp)
add_test(gtest gtest)
