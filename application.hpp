//
// Created by lihw on 26/12/22.
//

#pragma once

#include "Defines.hpp"

#include "camera.hpp"
#include "aqua_window.hpp"
#include "ADevice.hpp"
#include "renderer.hpp"
#include "game_object.hpp"
#include "simple_render_system.hpp"
#include "descriptor.hpp"

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

    private:
        AquaWindow _aquaWindow{WIDTH, HEIGHT, "Vulkan Demo"};
        ADevice _aquaDevice{_aquaWindow};
        Renderer _renderer{_aquaWindow, _aquaDevice};
        std::unique_ptr<DescriptorPool> _globalDescriptorPool{};
        std::vector<GameObject> _gameObjects;
    };
}