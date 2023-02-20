//
// Created by lihw on 20/02/23.
//

#include "ATexture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "vendors/stb_image.h"

namespace aqua
{
    ATexture::ATexture(AquaDevice& aquaDevice, const char* fileName) : _aquaDevice(aquaDevice)
    {
        createTextureImage(fileName);
        createTextureImageView();
        createTextureSampler();
    }

    ATexture::~ATexture()
    {
        vkDestroySampler(_aquaDevice.device(), _textureSampler, nullptr);
        vkDestroyImageView(_aquaDevice.device(), _textureImageView, nullptr);
        vkDestroyImage(_aquaDevice.device(), _textureImage, nullptr);
        vkFreeMemory(_aquaDevice.device(), _textureImageMemory, nullptr);
    }

    void ATexture::createTextureImage(const char* fileName)
    {
        // Read image data using stb_image
        int texWidth, texHeight, texChannel;
        stbi_uc* pixels = stbi_load(fileName, &texWidth, &texHeight, &texChannel, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels)
        {
            throw std::runtime_error("Failed to load texture image");
        }

        // Create staging buffer adn write image data to it
        Buffer stagingBuffer{
                _aquaDevice,
                imageSize,
                1,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*) pixels);
        stbi_image_free(pixels);

        // Create image
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = static_cast<uint32_t>(texWidth);
        imageCreateInfo.extent.height = static_cast<uint32_t>(texHeight);
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.flags = 0;

        _aquaDevice.createImageWithInfo(imageCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _textureImage,
                                        _textureImageMemory);

        // Copy data from staging buffer to image
        _aquaDevice.transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        _aquaDevice.copyBufferToImage(stagingBuffer.getBuffer(), _textureImage, static_cast<uint32_t>(texWidth),
                                      static_cast<uint32_t>(texHeight), 1);
        _aquaDevice.transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void ATexture::createTextureImageView()
    {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = _textureImage;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(_aquaDevice.device(), &imageViewCreateInfo, nullptr, &_textureImageView))
    }

    void ATexture::createTextureSampler()
    {
        VkSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.maxAnisotropy = _aquaDevice.properties().limits.maxSamplerAnisotropy;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = 0.0f;

        VK_CHECK(vkCreateSampler(_aquaDevice.device(), &samplerCreateInfo, nullptr, &_textureSampler))
    }
} // aqua