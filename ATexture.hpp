//
// Created by lihw on 20/02/23.
//

#pragma once

#include "Defines.hpp"
#include "ADevice.hpp"
#include "buffer.hpp"

namespace aqua
{
    class ATexture
    {
    public:
        explicit ATexture(ADevice& aquaDevice, const char* fileName);
        ATexture(const ATexture& aTexture) = delete;
        ~ATexture();
        ATexture& operator=(const ATexture& aTexture) = delete;

        [[nodiscard]] VkImageView TextureImageView()
        {
            return _textureImageView;
        }

        [[nodiscard]] VkSampler TextureSampler()
        {
            return _textureSampler;
        }

    private:
        void CreateTextureImage(const char* fileName);
        void CreateTextureImageView();
        void CreateTextureSampler();

    private:
        ADevice& _aquaDevice;
        VkImage _textureImage = VK_NULL_HANDLE;
        VkDeviceMemory _textureImageMemory = VK_NULL_HANDLE;
        VkImageView _textureImageView = VK_NULL_HANDLE;
        VkSampler _textureSampler = VK_NULL_HANDLE;
    };
} // aqua
