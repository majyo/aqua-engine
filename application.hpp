//
// Created by lihw on 26/12/22.
//

#pragma once

#include "camera.hpp"
#include "aqua_window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "game_object.hpp"
#include "simple_render_system.hpp"
#include "descriptor.hpp"

#include <memory>
#include <vector>
#include <chrono>

namespace aqua
{
    class Application
    {
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

        void createTextureImage();

    private:
        AquaWindow aquaWindow{WIDTH, HEIGHT, "Vulkan Demo"};
        AquaDevice device{aquaWindow};
        Renderer renderer{aquaWindow, device};
        std::unique_ptr<DescriptorPool> globalDescriptorPool{};
        std::vector<GameObject> gameObjects;

        // Temp
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
    };
}