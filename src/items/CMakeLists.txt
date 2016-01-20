include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR})

set(ITEMS_SRCS
  component.cc component.h component_builder.cc component_builder.h component_library.cc component_library.h
  ore.cc ore.h
  )

add_library(items STATIC ${ITEMS_SRCS})
target_include_directories(items PUBLIC "${SSI_SOURCE_DIR}/items")
target_link_libraries(items model ${LIBXML2_LIBRARIES})

