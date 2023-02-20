//
// Created by lihw on 29/12/22.
//

#pragma once

#include "Defines.h"

#include "device.hpp"
#include "buffer.hpp"

namespace aqua
{
    class Model
    {
    public:
        struct Vertex
        {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex& other) const
            {
                return position == other.position &&
                       color == other.color &&
                       normal == other.normal &&
                       uv == other.uv;
            }
        };

        struct Builder
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModel(const std::string& filename);
        };

        Model(AquaDevice& device, const Builder& builder);

        ~Model();

        Model(const Model& model) = delete;

        Model& operator=(const Model& model) = delete;

        void bind(VkCommandBuffer commandBuffer);

        void draw(VkCommandBuffer commandBuffer) const;

        static std::unique_ptr<Model> createModelFromFile(AquaDevice& device, const std::string& filePath);

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);

        void createIndexBuffers(const std::vector<uint32_t>& indices);

    private:
        AquaDevice& device;

//        VkBuffer vertexBuffer{};
//        VkDeviceMemory vertexBufferMemory{};
        std::unique_ptr<Buffer> vertexBuffer;
        uint32_t vertexCount{};

        bool hasIndexBuffer{false};
//        VkBuffer indexBuffer{};
//        VkDeviceMemory indexBufferMemory{};
        std::unique_ptr<Buffer> indexBuffer;
        uint32_t indexCount{};
    };
}
