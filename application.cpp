//
// Created by lihw on 26/12/22.
//

#include "application.hpp"

#include "keyboard_movement.hpp"
#include "ATexture.hpp"

namespace aqua
{
    struct GlobalUBO
    {
        glm::mat4 projectionView{1.0f};
        glm::vec4 lightDirection = glm::vec4(glm::normalize(glm::vec3(2.0f, 1.5f, 1.0f)), 0.0f);
        glm::vec4 viewPosition{0.0f, 0.0f, 1.0f, 0.0f};
    };

    Application::Application()
    {
        _globalDescriptorPool = DescriptorPool::Builder(_aquaDevice)
                .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();

        loadGameObject();
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        ATexture texture(_aquaDevice, "../Textures/wood_albedo.png");

        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (auto& uboBuffer: uboBuffers)
        {
            uboBuffer = std::make_unique<Buffer>(
                    _aquaDevice,
                    sizeof(GlobalUBO),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffer->map();
        }

        auto globalSetLayout = DescriptorSetLayout::Builder(_aquaDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < globalDescriptorSets.size(); ++i)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = texture.TextureImageView();
            imageInfo.sampler = texture.TextureSampler();

            DescriptorWriter(*globalSetLayout, *_globalDescriptorPool)
                    .writeBuffer(0, &bufferInfo)
                    .writeImage(1, &imageInfo)
                    .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{_aquaDevice, _renderer.getSwapChainRenderPass(),
                                              globalSetLayout->getDescriptorSetLayout()};
        Camera camera{};
        camera.setViewDirection(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f));

        auto viewer = GameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        KeyboardMovementController::SurroundingOrbit orbit{{0.0f, 0.0f, 0.0f}, 10.0f, glm::half_pi<float>(), 0.0f};

        while (!_aquaWindow.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

//            cameraController.moveInPlaneXZ(_aquaWindow.getGLFWWindow(), frameTime, viewer);
//            camera.setViewYXZ(viewer.modelMatrix.translation, viewer.mat.rotation);
            cameraController.moveEncircle(_aquaWindow.getGLFWWindow(), orbit, viewer);
            camera.setViewDirection(viewer.transform.translation, -viewer.transform.rotation,
                                    glm::vec3(0.0f, 1.0f, 0.0f));

            float aspectRatio = _renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 100.0f);

            if (auto commandBuffer = _renderer.beginFrame())
            {
                int frameIndex = _renderer.getFrameIndex();
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
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                _renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, _gameObjects);
                _renderer.endSwapChainRenderPass(commandBuffer);
                _renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(_aquaDevice.device());
    }

    void Application::loadGameObject()
    {
        std::shared_ptr<Model> model = Model::createModelFromFile(_aquaDevice, "../models/cube.obj");
        auto gameObject = GameObject::createGameObject();
        gameObject.model = model;
        gameObject.transform.scale = {1.0f, 1.0f, 1.0f};
//        gameObject.transform.scale = {7.0f, 7.0f, 7.0f};
//        gameObject.transform.rotation = {0.0f, 0.0f, glm::pi<float>()};
        _gameObjects.push_back(std::move(gameObject));
    }
}
