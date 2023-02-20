//
// Created by lihw on 26/12/22.
//

#include "application.hpp"

#include "keyboard_movement.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "vendors/stb_image.h"

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
        vkDestroySampler(_aquaDevice.device(), _textureSampler, nullptr);
        vkDestroyImageView(_aquaDevice.device(), _textureImageView, nullptr);
        vkDestroyImage(_aquaDevice.device(), _textureImage, nullptr);
        vkFreeMemory(_aquaDevice.device(), _textureImageMemory, nullptr);
    }

    void Application::run()
    {
        createTextureImage();
        createTextureImageView();
        createTextureSampler();

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
            imageInfo.imageView = _textureImageView;
            imageInfo.sampler = _textureSampler;

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

    void Application::createTextureImage()
    {
        // Read image data using stb_image
        int texWidth, texHeight, texChannel;
        stbi_uc* pixels = stbi_load("../Textures/wood_albedo.png", &texWidth, &texHeight, &texChannel, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels)
        {
            throw std::runtime_error("Failed to load texture image");
        }

        // Create staging buffer adn write image data to it
        Buffer stagingBuffer{
                _aquaDevice,
                imageSize,
                1,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*) pixels);
        stbi_image_free(pixels);

        // Create image
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

        _aquaDevice.createImageWithInfo(imageCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _textureImage,
                                        _textureImageMemory);

        // Copy data from staging buffer to image
        _aquaDevice.transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        _aquaDevice.copyBufferToImage(stagingBuffer.getBuffer(), _textureImage, static_cast<uint32_t>(texWidth),
                                      static_cast<uint32_t>(texHeight), 1);
        _aquaDevice.transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void Application::createTextureImageView()
    {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = _textureImage;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(_aquaDevice.device(), &imageViewCreateInfo, nullptr, &_textureImageView))
    }

    void Application::createTextureSampler()
    {
        VkSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.maxAnisotropy = _aquaDevice.properties().limits.maxSamplerAnisotropy;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 0.0f;

        VK_CHECK(vkCreateSampler(_aquaDevice.device(), &samplerCreateInfo, nullptr, &_textureSampler))
    }
}
