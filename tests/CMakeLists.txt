find_package(GTest REQUIRED)

add_executable(test_rtu test_rtu.cpp)
target_compile_options( test_rtu PUBLIC -Wall -Wextra -Werror)
set_property(TARGET test_rtu PROPERTY CXX_STANDARD 17)
set_property(TARGET test_rtu PROPERTY CXX_STANDARD_REQUIRED True)
target_link_libraries(test_rtu
    everest::modbus
    GTest::gtest
    GTest::gtest_main
    GTest::gmock)


add_executable( test_serial_helper test_serial_helper.cpp)
target_compile_options( test_serial_helper PUBLIC -Wall -Wextra -Werror)
set_property(TARGET test_serial_helper PROPERTY CXX_STANDARD 17)
set_property(TARGET test_serial_helper PROPERTY CXX_STANDARD_REQUIRED True)
target_link_libraries(test_serial_helper
    everest::modbus
    GTest::gtest
    GTest::gtest_main
    GTest::gmock)

enable_testing()

add_test(test_rtu test_rtu)
add_test(test_serial_helper test_serial_helper)
