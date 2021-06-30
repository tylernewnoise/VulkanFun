cmake_minimum_required(VERSION 3.17)
project(VulkanTut)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLM REQUIRED glm)
pkg_search_module(GLFW REQUIRED glfw3)

find_program(CLANG_FORMAT_EXECUTABLE
        NAMES clang-format
        DOC "Path to clang-format executable"
        )

file(GLOB SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
if (CLANG_FORMAT_EXECUTABLE)
    set(CLANG_FORMAT_ARGS "${CLANG_FORMAT_EXECUTABLE};-i;-style=Google")
    set(FILES_TO_FORMAT ${SOURCE_FILES})
    add_custom_target(${CMAKE_PROJECT_NAME}_format
            COMMAND ${CLANG_FORMAT_ARGS} ${FILES_TO_FORMAT})
else ()
    add_custom_target(${PROJECT_NAME}_format COMMAND "")
endif ()

find_program(GLSLANG_VALIDATOR_EXECUTABLE
        NAMES glslangValidator
        DOC "Path to glslangValidator executable"
        )

file(GLOB SHADER_FILES
        ${CMAKE_SOURCE_DIR}/data/shaders/*.frag
        ${CMAKE_SOURCE_DIR}/data/shaders/*.vert
        )
file(GLOB COMPILED_SHADER_FILES
        data/shaders/*.spv)
if (GLSLANG_VALIDATOR_EXECUTABLE)
    set(GLSLANG_ARGS "${GLSLANG_VALIDATOR_EXECUTABLE};-V")
    set(SHADERS_TO_COMPILE ${SHADER_FILES})
    add_custom_target(${CMAKE_PROJECT_NAME}_glslang
            COMMAND ${GLSLANG_ARGS} ${SHADERS_TO_COMPILE})
else ()
    message(WARNING "Shaders are not compiled!")
    add_custom_target(${PROJECT_NAME}_glslang COMMAND "")
endif ()

include_directories(${GLM_INCLUDE_DIRS})
include_directories($(GLFW_INCLUDE_DIRS))

add_executable(VulkanTut src/VulkanFun.cpp external/stb_image.h external/tiny_obj_loader.h)

add_dependencies(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_format)
add_dependencies(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_glslang)

target_link_libraries(VulkanTut Vulkan::Vulkan)
target_link_libraries(VulkanTut ${GLFW_LIBRARIES})

# https://www.glfw.org/docs/latest/build.html
# https://www.reddit.com/r/vulkan/comments/5873ic/psa_as_of_version_37_cmake_supports_vulkan_out_of/

