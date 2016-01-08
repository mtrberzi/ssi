find_package (Threads REQUIRED)
find_package (GTest REQUIRED)
include_directories (${GTEST_INCLUDE_DIRS})

add_executable (test_mcu test_mcu.cc)
target_link_libraries (test_mcu ${GTEST_BOTH_LIBRARIES} mcu)
add_test (TestMCU test_mcu)