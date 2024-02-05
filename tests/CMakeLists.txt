set(${TEST_TARGET_NAME} ${PROJECT_NAME}_tests)
add_executable(${TEST_TARGET_NAME}_rtu test_rtu.cpp)
target_link_libraries(${TEST_TARGET_NAME}_rtu
    PRIVATE
        everest::modbus
        GTest::gtest_main
        GTest::gmock
)


add_executable(${TEST_TARGET_NAME}_serial_helper test_serial_helper.cpp)
target_link_libraries(${TEST_TARGET_NAME}_serial_helper
    PRIVATE
        everest::modbus
        GTest::gtest_main
        GTest::gmock
)


include(GoogleTest)

gtest_discover_tests(${TEST_TARGET_NAME}_rtu)
gtest_discover_tests(${TEST_TARGET_NAME}_serial_helper)
