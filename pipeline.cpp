//
// Created by lihw on 26/12/22.
//

#include "pipeline.hpp"

#include <fstream>
#include <iostream>
#include <unistd.h>

namespace aqua {
    Pipeline::Pipeline(AquaDevice& device, const std::string &vertShaderPath, const std::string &fragShaderPath,
                       const PipelineConfigInfo& configInfo) : device(device) {
        char buffer[256];
        getcwd(buffer, 256);
        std::cout << buffer << std::endl;

        createGraphicsPipeline(vertShaderPath, fragShaderPath, configInfo);
    }

    std::vector<char> Pipeline::readFile(const std::string &filePath) {
        std::ifstream file{filePath, std::ios::ate | std::ios::binary};

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        auto fileSize = file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void Pipeline::createGraphicsPipeline(const std::string &vertShaderPath, const std::string &fragShaderPath,
                                          const PipelineConfigInfo& configInfo) {
        auto vertCode = readFile(vertShaderPath);
        auto fragCode = readFile(fragShaderPath);

        std::cout << "Vertex Shader Code Size: " << vertCode.size() << std::endl;
        std::cout << "Fragment Shader Code Size: " << fragCode.size() << std::endl;
    }

    void Pipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }
    }

    PipelineConfigInfo Pipeline::getDefaultPipelineConfigInfo(uint32_t width, uint32_t height) {
        PipelineConfigInfo configInfo{};
        return configInfo;
    }
}