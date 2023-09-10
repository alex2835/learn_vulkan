cmake_minimum_required(VERSION 3.12)

project(HelloTriangle VERSION 0.0.0)

add_executable(HelloTriangle 
"src/main.cpp"
"src/window.cpp"
"src/vulkan_renderer.cpp")

set_target_properties(
    HelloTriangle PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(HelloTriangle glfw)

include_directories( HelloTriangle PUBLIC $ENV{VULKAN_SDK}/include )
target_link_directories( HelloTriangle PUBLIC $ENV{VULKAN_SDK}/Lib )
target_link_libraries( HelloTriangle vulkan-1.lib )