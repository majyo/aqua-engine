//
// Created by lihw on 26/12/22.
//

#pragma once

#include "device.hpp"

#include <string>
#include <vector>

namespace aqua {
    struct PipelineConfigInfo {};

    class Pipeline {
    public:
        Pipeline(AquaDevice& device, const std::string& vertShaderPath, const std::string& fragShaderPath,
                 const PipelineConfigInfo& configInfo);
        ~Pipeline() {}
        Pipeline( const Pipeline& pipeline) = delete;
        Pipeline& operator=(const Pipeline& pipeline) = delete;

        static PipelineConfigInfo getDefaultPipelineConfigInfo(uint32_t width, uint32_t height);

    private:
        static std::vector<char> readFile(const std::string& filePath);
        void createGraphicsPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath,
                                    const PipelineConfigInfo& configInfo);
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
    private:
        AquaDevice& device;
        VkPipeline graphicsPipeline{};
        VkShaderModule vertShaderModule{};
        VkShaderModule fragShaderModule{};
    };
}
