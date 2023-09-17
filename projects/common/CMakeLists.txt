cmake_minimum_required(VERSION 3.12)

set(TARGET_NAME common)

file(GLOB_RECURSE SRCS
     "*.hpp"
     "*.cpp"
)
add_library(${TARGET_NAME} STATIC ${SRCS})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
