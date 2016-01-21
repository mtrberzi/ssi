find_package (Threads REQUIRED)
find_package (GTest REQUIRED)
include_directories (${GTEST_INCLUDE_DIRS})

## Items tests

add_executable (test_items_component_library test_items_component_library.cc)
target_link_libraries (test_items_component_library ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} items)
add_test (TestItems_ComponentLibrary test_items_component_library "${SSI_SOURCE_DIR}/../res/components.xml")

## MCU tests

add_executable (test_mcu test_mcu.cc)
target_link_libraries (test_mcu ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} mcu)
add_test (TestMCU test_mcu)

## Model tests

add_executable (test_model_material_library test_model_material_library.cc)
target_link_libraries (test_model_material_library ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} model)
add_test (TestModel_MaterialLibrary test_model_material_library "${SSI_SOURCE_DIR}/../res/materials.xml")

add_executable (test_model_transport_tubes test_model_transport_tubes.cc)
target_link_libraries (test_model_transport_tubes ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} model)
add_test (TestModel_TransportTubes test_model_transport_tubes)

add_executable (test_model_vector test_model_vector.cc)
target_link_libraries (test_model_vector ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} model)
add_test (TestModel_Vector test_model_vector)

add_custom_target (check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)