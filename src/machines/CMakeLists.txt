include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(MACHINES_SRCS
  smelter.h smelter.cc
)

add_library(machines STATIC ${MACHINES_SRCS})
target_include_directories(machines PUBLIC "${SSI_SOURCE_DIR}/machines")
target_link_libraries(machines model items)

