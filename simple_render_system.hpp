//
// Created by lihw on 12/01/23.
//

#pragma once

#include "Defines.h"

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "camera.hpp"
#include "frame_info.hpp"

namespace aqua
{
    class SimpleRenderSystem
    {
    public:
        SimpleRenderSystem(AquaDevice& device, VkRenderPass renderPass,
                           VkDescriptorSetLayout globalDescriptorSetLayout);

        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem& simpleRenderSystem) = delete;

        SimpleRenderSystem& operator=(const SimpleRenderSystem& simpleRenderSystem) = delete;

        void renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalDescriptorSetLayout);

        void createPipeline(VkRenderPass renderPass);

    private:
        AquaDevice& device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout{};
    };
}