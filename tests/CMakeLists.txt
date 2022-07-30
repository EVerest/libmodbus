option(USE_GTEST_LOCAL_ROLLING_RELEASE "use rolling release gtest version" OFF)

# this should go into a file that is read with something like include(gtest_settings.cmake OPTIONAL)
set(gtest_prefix ~/src/devtools/gtest/gtest-64bit-git-main-2022.07.30/)

set(gtest_include "${gtest_prefix}/include")
set(gtest_lib     "${gtest_prefix}/lib")

add_executable(test_rtu test_rtu.cpp)
target_compile_options( test_rtu PUBLIC -Wall -Wextra -Werror)

if (${USE_GTEST_LOCAL_ROLLING_RELEASE})
  target_include_directories(test_rtu PUBLIC ${gtest_include})
  target_link_directories(test_rtu    PUBLIC ${gtest_lib})
  # target_link_libraries(test_rtu everest::modbus gtest gtest_main gmock)
  target_link_libraries(test_rtu gtest gtest_main gmock)
else()
  find_package(GTest REQUIRED)
  target_link_libraries(test_rtu GTest::gtest GTest::gtest_main GTest::gmock)
endif()

target_link_libraries(test_rtu everest::modbus)

enable_testing()

add_test(test_rtu test_rtu)
