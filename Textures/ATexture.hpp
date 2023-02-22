//
// Created by lihw on 20/02/23.
//

#pragma once

#include "../Defines.hpp"
#include "../ADevice.hpp"
#include "../buffer.hpp"

namespace aqua
{
    class ATexture
    {
    public:
        class Builder
        {
        public:
            explicit Builder(ADevice& aDevice);

            std::unique_ptr<ATexture> Build();
            Builder& LoadFromFile(const char* filename);
            Builder& CreateTexture2D();
            Builder& CreateDepthStencil();
            Builder& CreateTexture2DSampler();

        public:
            bool createSampler{false};
            void* imageFileData = nullptr;
            VkImageCreateInfo imageCreateInfo{};
            VkImageViewCreateInfo imageViewCreateInfo{};
            VkSamplerCreateInfo samplerCreateInfo{};

        private:
            void InitCreateInfo();
            void CopyFileData(VkImage& image);

        private:
            ADevice& _aDevice;
        };

        ATexture(ADevice& aDevice, VkImage image, VkDeviceMemory memory, VkImageView imageView, VkSampler sampler);
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
        ADevice& _aDevice;
        VkImage _textureImage = VK_NULL_HANDLE;
        VkDeviceMemory _textureImageMemory = VK_NULL_HANDLE;
        VkImageView _textureImageView = VK_NULL_HANDLE;
        VkSampler _textureSampler = VK_NULL_HANDLE;
    };
} // aqua
