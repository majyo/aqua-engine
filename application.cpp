//
// Created by lihw on 26/12/22.
//

#include "application.hpp"

#include "keyboard_movement.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "vendors/stb_image.h"

#include <array>
#include <iostream>
#include <numeric>

namespace aqua {
    struct GlobalUBO {
        glm::mat4 projectionView{1.0f};
        glm::vec4 lightDirection = glm::vec4(glm::normalize(glm::vec3(2.0f, 1.5f, 1.0f)), 0.0f);
        glm::vec4 viewPosition{0.0f, 0.0f, 1.0f, 0.0f};
    };

    Application::Application() {
        globalDescriptorPool = DescriptorPool::Builder(device)
                .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();

        loadGameObject();
    }

    Application::~Application() {
    }

    void Application::run() {
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (auto& uboBuffer : uboBuffers) {
            uboBuffer = std::make_unique<Buffer>(
                    device,
                    sizeof(GlobalUBO),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffer->map();
        }

        auto globalSetLayout = DescriptorSetLayout::Builder(device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < globalDescriptorSets.size(); ++i) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *globalDescriptorPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
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
//            camera.setViewYXZ(viewer.modelMatrix.translation, viewer.mat.rotation);
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
                    camera,
                    globalDescriptorSets[frameIndex]
                };

                // update
                GlobalUBO ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                ubo.viewPosition = glm::vec4(viewer.transform.translation, 0.0f);
//                glm::vec3 test = viewer.transform.translation;
//                std::cout << test.x << ", " << test.y << ", " << test.z << std::endl;
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
        std::shared_ptr<Model> model = Model::createModelFromFile(device, "../models/cube.obj");
        auto gameObject = GameObject::createGameObject();
        gameObject.model = model;
        gameObject.transform.scale = {1.0f, 1.0f, 1.0f};
//        gameObject.transform.scale = {7.0f, 7.0f, 7.0f};
//        gameObject.transform.rotation = {0.0f, 0.0f, glm::pi<float>()};
        gameObjects.push_back(std::move(gameObject));
    }

    void Application::createTextureImage() {
        int texWidth, texHeight, texChannel;
        stbi_uc* pixels = stbi_load("../textures/texture.jpg", &texWidth, &texHeight, &texChannel, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            throw std::runtime_error("Failed to load texture image");
        }

        Buffer stagingBuffer{
            device,
            imageSize,
            1,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*) pixels);
        stbi_image_free(pixels);

        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = static_cast<uint32_t>(texWidth);
        imageCreateInfo.extent.height = static_cast<uint32_t>(texHeight);
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.flags = 0;

        device.createImageWithInfo(imageCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
    }
}
