//
// Created by lihw on 26/12/22.
//

#pragma once

#include "aqua_window.hpp"
#include "pipeline.hpp"
#include "device.hpp"

namespace aqua {
    class Application {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

    public:
        void run();

    private:
        AquaWindow aquaWindow{WIDTH, HEIGHT, "Vulkan Demo"};
        AquaDevice device{aquaWindow};
        Pipeline pipeline{device, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv",
                          Pipeline::getDefaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}