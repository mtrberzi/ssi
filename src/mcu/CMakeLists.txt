include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(MCU_SRCS rv32core.cc)

add_library(mcu STATIC ${MCU_SRCS})
target_include_directories(mcu PUBLIC "${ROOT_SOURCE_DIR}/mcu")
