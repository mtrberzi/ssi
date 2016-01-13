find_package (Threads REQUIRED)
find_package (GTest REQUIRED)
include_directories (${GTEST_INCLUDE_DIRS})

add_executable (test_mcu test_mcu.cc)
target_link_libraries (test_mcu ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} mcu)
add_test (TestMCU test_mcu)

add_executable (test_model_vector test_model_vector.cc)
target_link_libraries (test_model_vector ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} model)
add_test (TestModel_Vector test_model_vector)
