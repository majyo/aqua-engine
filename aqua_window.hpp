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
        [[nodiscard]] VkExtent2D getExtent() const {
            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        }
        [[nodiscard]] bool wasWindowResized() const {
            return frameBufferResized;
        }
        void resetWindowResizedFlag() {
            frameBufferResized = false;
        }
    private:
        void initWindow();
        static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
    private:
        int width;
        int height;
        bool frameBufferResized = false;
        std::string windowName;

        GLFWwindow* window{};
    };
}