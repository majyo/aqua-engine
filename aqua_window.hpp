//
// Created by lihw on 26/12/22.
//

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace aqua {
    class AquaWindow {
    public:
        AquaWindow(int width, int height, std::string name);
        ~AquaWindow();
        AquaWindow(const AquaWindow& window) = delete;
        AquaWindow& operator=(const AquaWindow& window) = delete;

        bool shouldClose() { return glfwWindowShouldClose(window); }
        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    private:
        void initWindow();
    private:
        const int width;
        const int height;
        std::string windowName;

        GLFWwindow* window{};
    };
}