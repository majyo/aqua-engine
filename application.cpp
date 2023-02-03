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
#include <numeric>

namespace aqua {
    struct GlobalUBO {
        glm::mat4 projectionView{1.0f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
    };

    Application::Application() {
        loadGameObject();
    }

    Application::~Application() {
    }

    void Application::run() {
//        auto minOffsetAlignment = std::lcm(
//                device.properties.limits.minUniformBufferOffsetAlignment,
//                device.properties.limits.nonCoherentAtomSize);
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); ++i) {
            uboBuffers[i] = std::make_unique<Buffer>(
                    device,
                    sizeof(GlobalUBO),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }
//        Buffer globalUniformBuffer{
//            device,
//            sizeof(GlobalUBO),
//            SwapChain::MAX_FRAMES_IN_FLIGHT,
//            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
//            minOffsetAlignment
//        };
//        globalUniformBuffer.map();

        SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};
        Camera camera{};
        camera.setViewDirection(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        auto viewer = GameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        KeyboardMovementController::SurroundingOrbit orbit{{0.0f, 0.0f, 0.0f}, 10.0f, glm::half_pi<float>(), 0.0f};

        while (!aquaWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

//            cameraController.moveInPlaneXZ(aquaWindow.getGLFWWindow(), frameTime, viewer);
//            camera.setViewYXZ(viewer.transform.translation, viewer.mat.rotation);
            cameraController.moveEncircle(aquaWindow.getGLFWWindow(), orbit, viewer);
            camera.setViewDirection(viewer.transform.translation, -viewer.transform.rotation, glm::vec3(0.0f, 1.0f, 0.0f));

            float aspectRatio = renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 100.0f);

            if (auto commandBuffer = renderer.beginFrame()) {
                int frameIndex = renderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera
                };

                // update
                GlobalUBO ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
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
//        gameObject.mat.scale = {1.0f, 1.0f, 1.0f};
        gameObject.transform.scale = {5.0f, 5.0f, 5.0f};
        gameObject.transform.rotation = {0.0f, 0.0f, glm::pi<float>()};
        gameObjects.push_back(std::move(gameObject));
    }
}
