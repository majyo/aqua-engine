//
// Created by lihw on 26/12/22.
//

#include "aqua_window.hpp"

#include <utility>
#include <stdexcept>

namespace aqua {
    AquaWindow::AquaWindow(int width, int height, std::string name)
    : width(width), height(height), windowName(std::move(name)) {
        initWindow();
    }

    AquaWindow::~AquaWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void AquaWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    void AquaWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
    }
}