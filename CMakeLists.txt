cmake_minimum_required(VERSION 3.12)

set (CMAKE_CXX_STANDARD 20)

include(FindVulkan)
if(NOT VULKAN_FOUND)
    message(FATAL_ERROR "Vulkan SDK not installed.")
endif()

# Dependencies
add_subdirectory(vendors)
# Projects
add_subdirectory(projects)
