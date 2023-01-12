//
// Created by lihw on 10/01/23.
//

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace aqua {
    class Camera {
    public:
        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float fov, float aspect, float near, float far);
        [[nodiscard]] const glm::mat4& getProjection() const {
            return projectionMatrix;
        }

    private:
        glm::mat4 projectionMatrix{1.0f};
    };
}