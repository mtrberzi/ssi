include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(MCU_SRCS rv32core.cc rv32core.h system_bus.cc system_bus.h)

add_library(mcu STATIC ${MCU_SRCS})
target_include_directories(mcu PUBLIC "${SSI_SOURCE_DIR}/mcu")
