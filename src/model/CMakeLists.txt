include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(MODEL_SRCS world.cc world.h vector.cc vector.h voxel_occupant.cc voxel_occupant.h transport_tube.cc transport_tube.h)

add_library(model STATIC ${MODEL_SRCS})
target_include_directories(model PUBLIC "${ROOT_SOURCE_DIR}/model")

