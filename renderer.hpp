//
// Created by lihw on 12/01/23.
//

#pragma once

#include "Defines.h"

#include "aqua_window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

namespace aqua
{
    class Renderer
    {
    public:
        Renderer(AquaWindow& window, AquaDevice& device);

        ~Renderer();

        Renderer(const Renderer& renderer) = delete;

        Renderer& operator=(const Renderer& renderer) = delete;

        VkCommandBuffer beginFrame();

        void endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

        void endSwapChainRenderPass(VkCommandBuffer commandBuffer) const;

        [[nodiscard]] bool isFrameInProgress() const
        {
            return isFrameStarted;
        }

        [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const
        {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        [[nodiscard]] VkRenderPass getSwapChainRenderPass() const
        {
            return swapChain->getRenderPass();
        }

        [[nodiscard]] int getFrameIndex() const
        {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        [[nodiscard]] float getAspectRatio() const
        {
            return swapChain->extentAspectRatio();
        }

    private:
        void recreateSwapChain();

        void createCommandBuffers();

        void freeCommandBuffers();

    private:
        AquaWindow& window;
        AquaDevice& device;
        std::unique_ptr<SwapChain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;
        int currentFrameIndex{0};
        uint32_t currentImageIndex{};
        bool isFrameStarted{false};
    };
}