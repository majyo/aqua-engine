#pragma once

#include "Defines.h"

#include "device.hpp"

namespace aqua
{

    class SwapChain
    {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        SwapChain(AquaDevice& deviceRef, VkExtent2D windowExtent);

        SwapChain(AquaDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previousSwapChain);

        ~SwapChain();

        SwapChain(const SwapChain&) = delete;

        SwapChain& operator=(const SwapChain&) = delete;

        VkFramebuffer getFrameBuffer(int index)
        { return swapChainFrameBuffers[index]; }

        VkRenderPass getRenderPass()
        { return renderPass; }

        VkImageView getImageView(int index)
        { return swapChainImageViews[index]; }

        size_t imageCount()
        { return swapChainImages.size(); }

        VkFormat getSwapChainImageFormat()
        { return swapChainImageFormat; }

        VkExtent2D getSwapChainExtent()
        { return swapChainExtent; }

        [[nodiscard]] uint32_t width() const
        { return swapChainExtent.width; }

        [[nodiscard]] uint32_t height() const
        { return swapChainExtent.height; }

        [[nodiscard]] float extentAspectRatio() const
        {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }

        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t* imageIndex);

        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

        [[nodiscard]] bool compareSwapChainFormats(const SwapChain& otherSwapChain) const
        {
            return otherSwapChain.swapChainImageFormat == swapChainImageFormat &&
                   otherSwapChain.swapChainDepthFormat == swapChainDepthFormat;
        }

    private:
        void init();

        void createSwapChain();

        void createImageViews();

        void createDepthResources();

        void createRenderPass();

        void createFrameBuffers();

        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR>& availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(
                const std::vector<VkPresentModeKHR>& availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFrameBuffers;
        VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemories;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        AquaDevice& device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;
        std::shared_ptr<SwapChain> previousSwapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
    };

}  // namespace aqua
