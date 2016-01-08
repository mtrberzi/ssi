include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(MODEL_SRCS world.cc)

add_library(model STATIC ${MODEL_SRCS})
target_include_directories(model PUBLIC "${ROOT_SOURCE_DIR}/model")
