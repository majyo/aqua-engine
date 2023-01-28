cmake_minimum_required(VERSION 3.22)
project(VulkanDemo)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

add_executable(VulkanDemo main.cpp aqua_window.hpp aqua_window.cpp application.hpp application.cpp pipeline.cpp pipeline.hpp device.hpp device.cpp swap_chain.hpp swap_chain.cpp model.cpp model.hpp game_object.hpp camera.hpp camera.cpp renderer.cpp renderer.hpp simple_render_system.hpp simple_render_system.cpp keyboard_movement.hpp keyboard_movement.cpp utils.hpp)

target_link_libraries(VulkanDemo glfw)
target_link_libraries(VulkanDemo vulkan)
target_link_libraries(VulkanDemo glm)
