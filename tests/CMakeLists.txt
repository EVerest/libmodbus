find_package(GTest REQUIRED)

add_executable(test_rtu test_rtu.cpp)

# target_compile_options( test_rtu PUBLIC -Wall -Wextra -Werror)
# compiler complains about "type qualifiers ignored on function return type" in connection/connection.hpp
# have a look at this later.
target_compile_options( test_rtu PUBLIC -Wall -Wextra )

target_link_libraries(test_rtu GTest::gtest GTest::gtest_main everest::modbus)

enable_testing()

add_test(test_rtu test_rtu)
