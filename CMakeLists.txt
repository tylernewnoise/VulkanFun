cmake_minimum_required(VERSION 3.13)
project(VulkanTut)

set(CMAKE_CXX_STANDARD 14)

find_package(Vulkan REQUIRED)

add_executable(VulkanTut VulkanTutorial.cpp)

target_link_libraries(VulkanTut Vulkan::Vulkan)


