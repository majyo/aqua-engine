//
// Created by lihw on 26/12/22.
//

#pragma once

#include "aqua_window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "game_object.hpp"

#include <memory>
#include <vector>

namespace aqua {
    class Application {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

    public:
        Application();
        ~Application();
        Application(const Application& application) = delete;
        Application& operator=(const Application& application) = delete;
        void run();

    private:
        void loadGameObject();
        void createPipelineLayout();
        void createPipeline();
//        void recreateSwapChain();
//        void createCommandBuffers();
//        void drawFrame();
//        void recordCommandBuffer(uint32_t imageIndex);
//        void freeCommandBuffers();
        void renderGameObject(VkCommandBuffer commandBuffer);

    private:
        AquaWindow aquaWindow{WIDTH, HEIGHT, "Vulkan Demo"};
        AquaDevice device{aquaWindow};
        Renderer renderer{aquaWindow, device};
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout{};
//        std::unique_ptr<SwapChain> swapChain;
//        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<GameObject> gameObjects;
    };
}