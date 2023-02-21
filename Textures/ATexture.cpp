//
// Created by lihw on 20/02/23.
//

#include "ATexture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../vendors/stb_image.h"

namespace aqua
{
    ATexture::Builder::Builder(aqua::ADevice& aDevice) : _aDevice(aDevice)
    {
    }

    std::unique_ptr<ATexture> ATexture::Builder::Build()
    {
        return std::unique_ptr<ATexture>();
    }

    std::unique_ptr<ATexture> ATexture::Builder::BuildFromFile(const char* filename)
    {
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkSampler imageSampler;

        stbi_set_flip_vertically_on_load(true);

        // Read image data using stb_image
        int texWidth, texHeight, texChannel;
        stbi_uc* pixels = stbi_load(filename, &texWidth, &texHeight, &texChannel, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels)
        {
            throw std::runtime_error("Failed to load texture image");
        }

        width = texWidth;
        height = texHeight;
        imageType = VK_IMAGE_TYPE_2D;
        format = VK_FORMAT_R8G8B8A8_SRGB;
        usages = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        // Create staging buffer adn write image data to it
        Buffer stagingBuffer{
                _aDevice,
                imageSize,
                1,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*) pixels);
        stbi_image_free(pixels);

        CreateImage(image, imageMemory);

        // Copy data from staging buffer to image
        _aDevice.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        _aDevice.copyBufferToImage(stagingBuffer.getBuffer(), image, static_cast<uint32_t>(texWidth),
                                   static_cast<uint32_t>(texHeight), 1);
        _aDevice.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        CreateImageView(image, imageView);
        CreateSampler(imageSampler);

        return std::make_unique<ATexture>(_aDevice, image, imageMemory, imageView, imageSampler);
    }

    void ATexture::Builder::CreateImage(VkImage& image, VkDeviceMemory& memory)
    {
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = imageType;
        imageCreateInfo.extent.width = width;
        imageCreateInfo.extent.height = height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = format;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = usages;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.flags = 0;

        _aDevice.createImageWithInfo(imageCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);
    }

    void ATexture::Builder::CreateImageView(VkImage image, VkImageView& imageView)
    {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(_aDevice.device(), &imageViewCreateInfo, nullptr, &imageView))
    }

    void ATexture::Builder::CreateSampler(VkSampler& imageSampler)
    {
        VkSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.maxAnisotropy = _aDevice.properties().limits.maxSamplerAnisotropy;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 0.0f;

        VK_CHECK(vkCreateSampler(_aDevice.device(), &samplerCreateInfo, nullptr, &imageSampler))
    }

    ATexture::ATexture(ADevice& aDevice, VkImage image, VkDeviceMemory memory, VkImageView imageView, VkSampler sampler)
    : _aDevice(aDevice), _textureImage(image), _textureImageMemory(memory), _textureImageView(imageView), _textureSampler(sampler)
    {
    }

    ATexture::~ATexture()
    {
        vkDestroySampler(_aDevice.device(), _textureSampler, nullptr);
        vkDestroyImageView(_aDevice.device(), _textureImageView, nullptr);
        vkDestroyImage(_aDevice.device(), _textureImage, nullptr);
        vkFreeMemory(_aDevice.device(), _textureImageMemory, nullptr);
    }
} // aqua