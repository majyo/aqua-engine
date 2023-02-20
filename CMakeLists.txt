cmake_minimum_required(VERSION 3.22)

set(NAME VulkanDemo)
project(${NAME})

set(CMAKE_CXX_STANDARD 20)

#set(SEP_BUILD TRUE)
#set(GLFW_PATH "C:/work/lib/glfw/mingw")
#set(VULKAN_SDK_PATH "C:/VulkanSDK/1.3.239.0")

if (DEFINED SEP_BUILD)
    set(GLFW_INCLUDE_DIR "${GLFW_PATH}/include")
    set(GLFW_LIBRARY "${GLFW_PATH}/lib")
    set(VULKAN_INCLUDE_DIR "${VULKAN_SDK_PATH}/Include")
    set(VULKAN_LIBRARY "${VULKAN_SDK_PATH}/Lib")
else()
    find_package(glfw3 3.3 REQUIRED)
    find_package(Vulkan REQUIRED)
    find_package(glm REQUIRED)
endif()

add_executable(VulkanDemo main.cpp aqua_window.hpp aqua_window.cpp application.hpp application.cpp pipeline.cpp pipeline.hpp device.hpp device.cpp swap_chain.hpp swap_chain.cpp model.cpp model.hpp game_object.hpp camera.hpp camera.cpp renderer.cpp renderer.hpp simple_render_system.hpp simple_render_system.cpp keyboard_movement.hpp keyboard_movement.cpp utils.hpp game_object.cpp buffer.cpp buffer.hpp frame_info.hpp descriptor.hpp descriptor.cpp ATexture.cpp ATexture.hpp)

target_precompile_headers(VulkanDemo PRIVATE Defines.hpp)

if (DEFINED SEP_BUILD)
    target_include_directories(${PROJECT_NAME} PUBLIC
            ${GLFW_INCLUDE_DIR}
            ${VULKAN_INCLUDE_DIR})
    target_link_directories(${PROJECT_NAME} PUBLIC
            ${GLFW_LIBRARY}
            ${VULKAN_LIBRARY})
    target_link_libraries(${PROJECT_NAME} glfw3 vulkan-1)
else()
    target_link_libraries(VulkanDemo glfw)
    target_link_libraries(VulkanDemo vulkan)
    target_link_libraries(VulkanDemo glm)
endif()
