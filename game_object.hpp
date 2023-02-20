//
// Created by lihw on 04/01/23.
//

#pragma once

#include "Defines.h"

#include "model.hpp"

namespace aqua
{
    struct TransformComponent
    {
    public:
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        [[nodiscard]] glm::mat4 mat() const;

        [[nodiscard]] glm::mat3 normalMat() const;
    };

    class GameObject
    {
    public:
        using id_t = unsigned int;

    public:
        static GameObject createGameObject()
        {
            static id_t currentId = 0;
            return GameObject{currentId++};
        }

        GameObject(const GameObject& gameObject) = delete;

        GameObject& operator=(const GameObject& gameObject) = delete;

        GameObject(GameObject&& gameObject) = default;

        GameObject& operator=(GameObject&& gameObject) = default;

        [[nodiscard]] id_t getId() const
        {
            return id;
        }

    public:
        std::shared_ptr<aqua::Model> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        explicit GameObject(id_t id) : id(id)
        {}

    private:
        id_t id;
    };
}