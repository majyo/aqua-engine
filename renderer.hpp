//
// Created by lihw on 12/01/23.
//

#pragma once

#include "aqua_window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace aqua {
    class Renderer {
    public:
        Renderer(AquaWindow& window, AquaDevice& device);
        ~Renderer();
        Renderer(const Renderer &renderer) = delete;
        Renderer &operator=(const Renderer &renderer) = delete;

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer) const;

        [[nodiscard]] bool isFrameInProgress() const {
            return isFrameStarted;
        }

        [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentImageIndex];
        }

        [[nodiscard]] VkRenderPass getSwapChainRenderPass() const {
            return swapChain->getRenderPass();
        }

    private:
        void recreateSwapChain();
        void createCommandBuffers();
        void drawFrame();
        void freeCommandBuffers();

    private:
        AquaWindow& window;
        AquaDevice& device;
        std::unique_ptr<SwapChain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;
        uint32_t currentImageIndex{};
        bool isFrameStarted{false};
    };
}