include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR})

set(MODEL_SRCS 
  world.cc world.h world_updates.cc world_updates.h time_constants.h vector.cc vector.h
  voxel_occupant.cc voxel_occupant.h machine.h machine.cc structures.cc structures.h
  transport_device.h transport_tube.cc transport_tube.h transport_endpoint.cc transport_endpoint.h
  material.cc material.h material_library.cc material_library.h material_builder.cc material_builder.h
  uuid.cc uuid.h)

add_library(model STATIC ${MODEL_SRCS})
target_include_directories(model PUBLIC "${SSI_SOURCE_DIR}/model")
target_link_libraries(model ${LIBXML2_LIBRARIES})

