find_package(GTest REQUIRED)

add_executable(test_rtu test_rtu.cpp)

# this implies some little changed related to "type qualifiers ignored on function return type" in connection/connection.hpp when enabling -Werror
target_compile_options( test_rtu PUBLIC -Wall -Wextra -Werror)

target_link_libraries(test_rtu GTest::gtest GTest::gtest_main everest::modbus)

enable_testing()

add_test(test_rtu test_rtu)
