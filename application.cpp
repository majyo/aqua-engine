//
// Created by lihw on 26/12/22.
//

#include "application.hpp"
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct SimplePushConstantData {
    glm::mat2 transform{1.0f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

namespace aqua {

    Application::Application() {
        loadGameObject();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    Application::~Application() {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void Application::run() {
        while (!aquaWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(device.device());
    }

    void Application::loadGameObject() {
        std::vector<Model::Vertex> vertices {
                {{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        auto model = std::make_shared<Model>(device, vertices);
        auto triangle = GameObject::createGameObject();
        triangle.model = model;
        triangle.color = {0.0f, 0.0f, 0.5f};
        triangle.transform2D.translation.x = 0.2f;
        triangle.transform2D.scale = {2.0f, 0.5f};
        triangle.transform2D.rotation = 0.25f * glm::two_pi<float>();
        gameObjects.push_back(std::move(triangle));
    }

    void Application::createPipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) !=VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        }
    }

    void Application::createPipeline() {
        assert(swapChain != nullptr && "Cannot create pipeline before swap chain.");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");
//        auto pipelineConfig = Pipeline::setAsDefaultPipelineConfigInfo(swapChain->width(), swapChain->height());
        PipelineConfigInfo pipelineConfigInfo{};
        Pipeline::setAsDefaultPipelineConfigInfo(pipelineConfigInfo);
        pipelineConfigInfo.renderPass = swapChain->getRenderPass();
        pipelineConfigInfo.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
                device,
                "../shaders/simple_shader.vert.spv",
                "../shaders/simple_shader.frag.spv",
                pipelineConfigInfo);
    }

    void Application::recreateSwapChain() {
        auto extent = aquaWindow.getExtent();

        while (extent.width == 0 || extent.height == 0) {
            extent = aquaWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device.device());

        if (swapChain == nullptr) {
            swapChain = std::make_unique<SwapChain>(device, extent);
        } else {
            swapChain = std::make_unique<SwapChain>(device, extent, std::move(swapChain));

            if (swapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        createPipeline();
    }

    void Application::createCommandBuffers() {
        commandBuffers.resize(swapChain->imageCount());
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = device.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers");
        }
    }

    void Application::recordCommandBuffer(uint32_t imageIndex) {
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &commandBufferBeginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = swapChain->getRenderPass();
        renderPassBeginInfo.framebuffer = swapChain->getFrameBuffer((int) imageIndex);

        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = swapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color =  {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChain->getSwapChainExtent();

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        renderGameObject(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);

        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to close command buffer");
        }
    }

    void Application::drawFrame() {
        uint32_t imageIndex;
        auto result = swapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image");
        }

        recordCommandBuffer(imageIndex);
        result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || aquaWindow.wasWindowResized()) {
            aquaWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image");
        }
    }

    void Application::freeCommandBuffers() {
        vkFreeCommandBuffers(device.device(), device.getCommandPool(), commandBuffers.size(), commandBuffers.data());
        commandBuffers.clear();
    }

    void Application::renderGameObject(VkCommandBuffer commandBuffer) {
        pipeline->bind(commandBuffer);

        for (auto& obj : gameObjects) {
            SimplePushConstantData pushConstantData{};
            pushConstantData.offset = obj.transform2D.translation;
            pushConstantData.color = obj.color;
            pushConstantData.transform = obj.transform2D.mat2();

            vkCmdPushConstants(
                    commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(SimplePushConstantData),
                    &pushConstantData);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
}