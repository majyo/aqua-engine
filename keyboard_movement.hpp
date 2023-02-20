//
// Created by lihw on 16/01/23.
//

#pragma once

#include "Defines.h"

#include "game_object.hpp"
#include "aqua_window.hpp"

namespace aqua
{
    class KeyboardMovementController
    {
    public:
        struct KeyMappings
        {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        struct SurroundingOrbit
        {
            glm::vec3 focus;
            float radius;
            float theta;
            float phi;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, GameObject& gameObject);

        void moveEncircle(GLFWwindow* window, SurroundingOrbit& orbit, GameObject& gameObject);

    public:
        KeyMappings keyMappings{};
        float moveSpeed{3.0f};
        float lookSpeed{1.0f};
        double mouseX{0.0};
        double mouseY{0.0};
    };
}
