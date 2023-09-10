cmake_minimum_required(VERSION 3.12)

set(TARGET_NAME HelloTriangle)

file(GLOB_RECURSE SRCS
     "*.hpp"
     "*.cpp"
)
add_executable(${TARGET_NAME} ${SRCS})

target_include_directories(${TARGET_NAME} PUBLIC include)
target_link_libraries(${TARGET_NAME} Vulkan::Vulkan)
target_link_libraries(${TARGET_NAME} glfw)
target_link_libraries(${TARGET_NAME} glm::glm)
target_link_libraries(${TARGET_NAME} imgui)

# Shaders
include(${CMAKE_SOURCE_DIR}/cmake/embed-data.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/compile_shaders.cmake)
file(GLOB_RECURSE SHADERS "${CMAKE_SOURCE_DIR}/shaders/*.vert" "${CMAKE_SOURCE_DIR}/shaders/*.frag")
compile_shaders(${TARGET_NAME} ${SHADERS})
