//
// Created by lihw on 04/01/23.
//

#pragma once

#include "model.hpp"
#include "memory"

namespace aqua {
    struct Transform2DComponent {
    public:
        glm::vec2 translation{};
        glm::vec2 scale{1.0f, 1.0f};
        float rotation;

        [[nodiscard]] glm::mat2 mat2() const {
            glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
            const float sinR = glm::sin(rotation);
            const float cosR = glm::cos(rotation);
            glm::mat2 rotMat{{cosR, sinR}, {-sinR, cosR}};
            return rotMat * scaleMat;
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
        Transform2DComponent transform2D{};

    private:
        explicit GameObject(id_t id) : id(id) {}

    private:
        id_t id;
    };
}