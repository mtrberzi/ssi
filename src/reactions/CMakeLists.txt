include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR})

set(REACTIONS_SRCS
  reaction.cc reaction.h reaction_builder.cc reaction_builder.h reaction_library.cc reaction_library.h
  reactant.cc reactant.h product.cc product.h
  reactant_constraint.cc reactant_constraint.h
  )

add_library(reactions STATIC ${REACTIONS_SRCS})
target_include_directories(reactions PUBLIC "${SSI_SOURCE_DIR}/reactions")
target_link_libraries(reactions model ${LIBXML2_LIBRARIES})

