//
// Created by lihw on 22/02/23.
//

#pragma once

#include "Textures/ATexture.hpp"

#include "Defines.hpp"

namespace aqua
{
    class ARenderPass
    {
    private:
        VkPipeline _pipeline;
        VkFramebuffer _framebuffer;
        std::unique_ptr<ATexture> _framebufferAttachment;
        VkRenderPass _renderPass;
    };
} // aqua
