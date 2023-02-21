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
            std::unique_ptr<ATexture> BuildFromFile(const char* filename);
            std::unique_ptr<ATexture> BuildTexture2D();
            std::unique_ptr<ATexture> BuildDepthStencil();

        public:
            uint32_t width{};
            uint32_t height{};
            VkImageType imageType{};
            VkFormat format{};
            VkImageUsageFlags usages{};

        private:
            void* LoadImageData(const char* filename);
            void CreateImage(VkImage& image, VkDeviceMemory& memory);
            void CreateImageView(VkImage image, VkImageView& imageView);
            void CreateSampler(VkSampler& imageSampler);

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
