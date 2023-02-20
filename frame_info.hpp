//
// Created by lihw on 03/02/23.
//

#pragma once

#include "camera.hpp"

#include <vulkan/vulkan.h>

namespace aqua
{
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera& camera;
        VkDescriptorSet globalDescriptorSet;
    };
}
