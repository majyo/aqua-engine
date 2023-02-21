//
// Created by lihw on 21/02/23.
//

#pragma once

#include "ADevice.hpp"

namespace aqua
{
    class ATextureSampler
    {
    public:
        explicit ATextureSampler(ADevice& device);
        ATextureSampler(const ATextureSampler& textureSampler) = delete;
        ATextureSampler& operator=(const ATextureSampler& textureSampler) = delete;
        ~ATextureSampler();

    private:
        ADevice& _aquaDevice;
        VkSampler _textureSampler = VK_NULL_HANDLE;
    };
}
