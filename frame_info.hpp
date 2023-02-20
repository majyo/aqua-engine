//
// Created by lihw on 03/02/23.
//

#pragma once

#include "Defines.h"

#include "camera.hpp"

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
