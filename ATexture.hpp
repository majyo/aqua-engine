//
// Created by lihw on 20/02/23.
//

#pragma once

#include "Defines.hpp"
#include "device.hpp"
#include "buffer.hpp"

namespace aqua
{
    class ATexture
    {
    public:
        explicit ATexture(AquaDevice& aquaDevice, const char* fileName);
    private:
        void createTextureImage(const char* fileName);
        void createTextureImageView();
        void createTextureSampler();
    private:
        AquaDevice& _aquaDevice;
        VkImage _textureImage = VK_NULL_HANDLE;
        VkDeviceMemory _textureImageMemory = VK_NULL_HANDLE;
        VkImageView _textureImageView = VK_NULL_HANDLE;
        VkSampler _textureSampler = VK_NULL_HANDLE;
    };
} // aqua
