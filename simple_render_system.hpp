//
// Created by lihw on 12/01/23.
//

#pragma once

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "camera.hpp"

#include <memory>
#include <vector>

namespace aqua {
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(AquaDevice& device, VkRenderPass renderPass);
        ~SimpleRenderSystem();
        SimpleRenderSystem(const SimpleRenderSystem& simpleRenderSystem) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem& simpleRenderSystem) = delete;

        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects,
                               const Camera& camera);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

    private:
        AquaDevice& device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout{};
    };
}