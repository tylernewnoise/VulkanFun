cmake_minimum_required(VERSION 3.10)

project(VulkanTut)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLM REQUIRED glm)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(${GLM_INCLUDE_DIRS})
include_directories($(GLFW_INCLUDE_DIRS))

add_executable(VulkanTut src/VulkanTutorial.cpp external/stb_image.h external/tiny_obj_loader.h)

target_link_libraries(VulkanTut Vulkan::Vulkan)
target_link_libraries(VulkanTut ${GLFW_LIBRARIES})

# https://www.glfw.org/docs/latest/build.html
# https://www.reddit.com/r/vulkan/comments/5873ic/psa_as_of_version_37_cmake_supports_vulkan_out_of/

