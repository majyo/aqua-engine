//
// Created by lihw on 26/12/22.
//

#include "application.hpp"
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace aqua {
    std::unique_ptr<Model> createCubeModel(AquaDevice& device, glm::vec3 offset);

    Application::Application() {
        loadGameObject();
//        createPipelineLayout();
//        createPipeline();
    }

    Application::~Application() {
//        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void Application::run() {
        SimpleRenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};

        while (!aquaWindow.shouldClose()) {
            glfwPollEvents();

            if (auto commandBuffer = renderer.beginFrame()) {
                renderer.beginSwapChainRenderPass(commandBuffer);
//                renderGameObject(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(device.device());
    }

    void Application::loadGameObject() {
        std::shared_ptr<Model> model = createCubeModel(device, {0.0f, 0.0f, 0.0f});
        auto cube = GameObject::createGameObject();
        cube.model = model;
        cube.color = {0.0f, 0.25f, 0.25f};
        cube.transform.translation.z = 0.5f;
        cube.transform.scale = {0.5f, 0.5f, 0.5f};
        gameObjects.push_back(std::move(cube));
    }

//    void Application::createPipelineLayout() {
//        VkPushConstantRange pushConstantRange{};
//        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
//        pushConstantRange.offset = 0;
//        pushConstantRange.size = sizeof(SimplePushConstantData);
//
//        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
//        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//        pipelineLayoutCreateInfo.setLayoutCount = 0;
//        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
//        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
//        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
//
//        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) !=VK_SUCCESS) {
//            throw std::runtime_error("Failed to create pipeline layout");
//        }
//    }
//
//    void Application::createPipeline() {
//        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");
//
//        PipelineConfigInfo pipelineConfigInfo{};
//        Pipeline::setAsDefaultPipelineConfigInfo(pipelineConfigInfo);
//        pipelineConfigInfo.renderPass = renderer.getSwapChainRenderPass();
//        pipelineConfigInfo.pipelineLayout = pipelineLayout;
//        pipeline = std::make_unique<Pipeline>(
//                device,
//                "../shaders/simple_shader.vert.spv",
//                "../shaders/simple_shader.frag.spv",
//                pipelineConfigInfo);
//    }
//
//    void Application::renderGameObject(VkCommandBuffer commandBuffer) {
//        pipeline->bind(commandBuffer);
//
//        for (auto& obj : gameObjects) {
//            obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
//            obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.01f, glm::two_pi<float>());
//
//            SimplePushConstantData pushConstantData{};
//            pushConstantData.transform = obj.transform.transform();
//            pushConstantData.color = obj.color;
//
//            vkCmdPushConstants(
//                    commandBuffer,
//                    pipelineLayout,
//                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
//                    0,
//                    sizeof(SimplePushConstantData),
//                    &pushConstantData);
//            obj.model->bind(commandBuffer);
//            obj.model->draw(commandBuffer);
//        }
//    }

    std::unique_ptr<Model> createCubeModel(AquaDevice& device, glm::vec3 offset) {
        std::vector<Model::Vertex> vertices{
                // left face (white)
                {{-.5f, -.5f, -.5f},  {.9f, .9f, .9f}},
                {{-.5f, .5f,  .5f},   {.9f, .9f, .9f}},
                {{-.5f, -.5f, .5f},   {.9f, .9f, .9f}},
                {{-.5f, -.5f, -.5f},  {.9f, .9f, .9f}},
                {{-.5f, .5f,  -.5f},  {.9f, .9f, .9f}},
                {{-.5f, .5f,  .5f},   {.9f, .9f, .9f}},

                // right face (yellow)
                {{.5f,  -.5f, -.5f},  {.8f, .8f, .1f}},
                {{.5f,  .5f,  .5f},   {.8f, .8f, .1f}},
                {{.5f,  -.5f, .5f},   {.8f, .8f, .1f}},
                {{.5f,  -.5f, -.5f},  {.8f, .8f, .1f}},
                {{.5f,  .5f,  -.5f},  {.8f, .8f, .1f}},
                {{.5f,  .5f,  .5f},   {.8f, .8f, .1f}},

                // top face (orange, remember y-axis points down)
                {{-.5f, -.5f, -.5f},  {.9f, .6f, .1f}},
                {{.5f,  -.5f, .5f},   {.9f, .6f, .1f}},
                {{-.5f, -.5f, .5f},   {.9f, .6f, .1f}},
                {{-.5f, -.5f, -.5f},  {.9f, .6f, .1f}},
                {{.5f,  -.5f, -.5f},  {.9f, .6f, .1f}},
                {{.5f,  -.5f, .5f},   {.9f, .6f, .1f}},

                // bottom face (red)
                {{-.5f, .5f,  -.5f},  {.8f, .1f, .1f}},
                {{.5f,  .5f,  .5f},   {.8f, .1f, .1f}},
                {{-.5f, .5f,  .5f},   {.8f, .1f, .1f}},
                {{-.5f, .5f,  -.5f},  {.8f, .1f, .1f}},
                {{.5f,  .5f,  -.5f},  {.8f, .1f, .1f}},
                {{.5f,  .5f,  .5f},   {.8f, .1f, .1f}},

                // nose face (blue)
                {{-.5f, -.5f, 0.5f},  {.1f, .1f, .8f}},
                {{.5f,  .5f,  0.5f},  {.1f, .1f, .8f}},
                {{-.5f, .5f,  0.5f},  {.1f, .1f, .8f}},
                {{-.5f, -.5f, 0.5f},  {.1f, .1f, .8f}},
                {{.5f,  -.5f, 0.5f},  {.1f, .1f, .8f}},
                {{.5f,  .5f,  0.5f},  {.1f, .1f, .8f}},

                // tail face (green)
                {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f,  .5f,  -0.5f}, {.1f, .8f, .1f}},
                {{-.5f, .5f,  -0.5f}, {.1f, .8f, .1f}},
                {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f,  -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f,  .5f,  -0.5f}, {.1f, .8f, .1f}},
        };

        for (auto &v: vertices) {
            v.position += offset;
        }

        return std::make_unique<Model>(device, vertices);
    }
}
