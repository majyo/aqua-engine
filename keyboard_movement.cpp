//
// Created by lihw on 16/01/23.
//

#include "keyboard_movement.hpp"

#include <cmath>
#include <iostream>

void aqua::KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, aqua::GameObject& gameObject)
{
    glm::vec3 rotation{0.0f};

    if (glfwGetKey(window, keyMappings.lookRight) == GLFW_PRESS)
    { rotation.y += 1.0f; }
    if (glfwGetKey(window, keyMappings.lookLeft) == GLFW_PRESS)
    { rotation.y -= 1.0f; }
    if (glfwGetKey(window, keyMappings.lookUp) == GLFW_PRESS)
    { rotation.x += 1.0f; }
    if (glfwGetKey(window, keyMappings.lookDown) == GLFW_PRESS)
    { rotation.x -= 1.0f; }

    if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon())
    {
        gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotation);
    }

    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDirection{std::sin(yaw), 0.0f, std::cos(yaw)};
    const glm::vec3 rightDirection{forwardDirection.z, 0.0f, -forwardDirection.x};
    const glm::vec3 upDirection{0.0f, -1.0f, 0.0f};

    glm::vec3 moveDirection{0.0f};

    if (glfwGetKey(window, keyMappings.moveForward) == GLFW_PRESS)
    { moveDirection += forwardDirection; }
    if (glfwGetKey(window, keyMappings.moveBackward) == GLFW_PRESS)
    { moveDirection -= forwardDirection; }
    if (glfwGetKey(window, keyMappings.moveRight) == GLFW_PRESS)
    { moveDirection += rightDirection; }
    if (glfwGetKey(window, keyMappings.moveLeft) == GLFW_PRESS)
    { moveDirection -= rightDirection; }
    if (glfwGetKey(window, keyMappings.moveUp) == GLFW_PRESS)
    { moveDirection += upDirection; }
    if (glfwGetKey(window, keyMappings.moveDown) == GLFW_PRESS)
    { moveDirection -= upDirection; }

    if (glm::dot(moveDirection, moveDirection) > std::numeric_limits<float>::epsilon())
    {
        gameObject.transform.translation += moveSpeed * dt * moveDirection;
    }
}

void aqua::KeyboardMovementController::moveEncircle(GLFWwindow* window, SurroundingOrbit& orbit,
                                                    aqua::GameObject& gameObject)
{
    auto prevMouseX = mouseX;
    auto prevMouseY = mouseY;

    glfwGetCursorPos(window, &mouseX, &mouseY);
    auto deltaMouseX = mouseX - prevMouseX;
    auto deltaMouseY = mouseY - prevMouseY;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        orbit.phi += static_cast<float>(-deltaMouseX) * 0.004f;
        orbit.theta += static_cast<float>(-deltaMouseY) * 0.004f;
    }

    orbit.theta = glm::clamp(orbit.theta, 0.2f, 2.8f);
    orbit.phi = glm::mod(orbit.phi, glm::two_pi<float>());


    glm::vec3 direction{glm::sin(orbit.theta) * glm::sin(orbit.phi),
                        glm::cos(orbit.theta),
                        glm::sin(orbit.theta) * glm::cos(orbit.phi)};
    glm::vec3 position = orbit.focus + direction * orbit.radius;

    gameObject.transform.translation = position;
    gameObject.transform.rotation = direction;
}
