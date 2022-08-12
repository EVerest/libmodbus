add_executable(test_rtu test_rtu.cpp)
target_link_libraries(test_rtu
    PRIVATE
        everest::modbus
        GTest::gtest_main
        GTest::gmock
)


add_executable(test_serial_helper test_serial_helper.cpp)
target_link_libraries(test_serial_helper
    PRIVATE
        everest::modbus
        GTest::gtest_main
        GTest::gmock
)


include(GoogleTest)

gtest_discover_tests(test_rtu)
gtest_discover_tests(test_serial_helper)
