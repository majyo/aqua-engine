//
// Created by lihw on 26/12/22.
//

#pragma once

#include "Defines.h"

#include "device.hpp"
#include "model.hpp"

namespace aqua
{
    struct PipelineConfigInfo
    {
        PipelineConfigInfo() = default;

        PipelineConfigInfo(const PipelineConfigInfo& pipelineConfigInfo) = delete;

        PipelineConfigInfo& operator=(const PipelineConfigInfo& pipelineConfigInfo) = delete;

//        VkViewport viewport{};
//        VkRect2D scissor{};
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        VkPipelineMultisampleStateCreateInfo multisampleInfo{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        std::vector<VkDynamicState> dynamicStateEnables{};
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline
    {
    public:
        Pipeline(AquaDevice& device, const std::string& vertShaderPath, const std::string& fragShaderPath,
                 const PipelineConfigInfo& configInfo);

        ~Pipeline();

        Pipeline(const Pipeline& pipeline) = delete;

        Pipeline& operator=(const Pipeline& pipeline) = delete;

        void bind(VkCommandBuffer commandBuffer);

//        static PipelineConfigInfo setAsDefaultPipelineConfigInfo(uint32_t width, uint32_t height);
        static void setAsDefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

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
