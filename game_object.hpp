//
// Created by lihw on 04/01/23.
//

#pragma once

#include "model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace aqua {
    struct TransformComponent {
    public:
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        [[nodiscard]] glm::mat4 transform() const {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);

            return glm::mat4{
                    {
                        scale.x * (c1 * c3 + s1 * s2 * s3),
                        scale.x * (c2 * s3),
                        scale.x * (c1 * s2 * s3 - c3 * s1),
                        0.0f,
                    },
                    {
                        scale.y * (c3 * s1 * s2 - c1 * s3),
                        scale.y * (c2 * c3),
                        scale.y * (c1 * c3 * s2 + s1 * s3),
                        0.0f,
                    },
                    {
                        scale.z * (c2 * s1),
                        scale.z * (-s2),
                        scale.z * (c1 * c2),
                        0.0f,
                    },
                    {
                        translation.x,
                        translation.y,
                        translation.z,
                        1.0f
                    }
            };

//            auto transform = glm::translate(glm::mat4{1.0f}, translation);
//            transform = glm::rotate(transform, rotation.y, {0.0f, 1.0f, 0.0f});
//            transform = glm::rotate(transform, rotation.x, {1.0f, 0.0f, 0.0f});
//            transform = glm::rotate(transform, rotation.z, {0.0f, 0.0f, 1.0f});
//            transform = glm::scale(transform, scale);
//            return transform;
        }
    };

    class GameObject {
    public:
        using id_t = unsigned int;

    public:
        static GameObject createGameObject() {
            static id_t currentId = 0;
            return GameObject{currentId++};
        }

        GameObject(const GameObject& gameObject) = delete;
        GameObject& operator=(const GameObject& gameObject) = delete;
        GameObject(GameObject&& gameObject) = default;
        GameObject& operator=(GameObject&& gameObject) = default;

        [[nodiscard]] id_t getId() const {
            return id;
        }

    public:
        std::shared_ptr<aqua::Model> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        explicit GameObject(id_t id) : id(id) {}

    private:
        id_t id;
    };
}