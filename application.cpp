//
// Created by lihw on 26/12/22.
//

#include "application.hpp"

#include "keyboard_movement.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <iostream>

namespace aqua {
    Application::Application() {
        loadGameObject();
    }

    Application::~Application() {
    }

    void Application::run() {
        SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};
        Camera camera{};
        camera.setViewDirection(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, -1.0f));

        auto viewer = GameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        KeyboardMovementController::SurroundingOrbit orbit{{0.0f, 0.0f, 0.0f}, 2.0f, glm::half_pi<float>(), 0.0f};

        while (!aquaWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

//            cameraController.moveInPlaneXZ(aquaWindow.getGLFWWindow(), frameTime, viewer);
//            camera.setViewYXZ(viewer.transform.translation, viewer.transform.rotation);
            cameraController.moveEncircle(aquaWindow.getGLFWWindow(), orbit, viewer);
            camera.setViewDirection(viewer.transform.translation, -viewer.transform.rotation);

            float aspectRatio = renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 10.0f);

            if (auto commandBuffer = renderer.beginFrame()) {
                renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(device.device());
    }

    void Application::loadGameObject() {
        std::shared_ptr<Model> model = Model::createModelFromFile(device, "../models/smooth_vase.obj");
        auto gameObject = GameObject::createGameObject();
        gameObject.model = model;
        gameObject.transform.translation.z = 0.0f;
        gameObject.transform.scale = {1.0f, 1.0f, 1.0f};
        gameObjects.push_back(std::move(gameObject));
    }
}
