//
// Created by lihw on 26/12/22.
//

#include "application.hpp"

namespace aqua {
    void Application::run() {
        while (aquaWindow.shouldClose()) {
            glfwPollEvents();
        }
    }
}