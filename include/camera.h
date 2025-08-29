#pragma once
#include <algorithm>
#include <GLFW/glfw3.h>
#include "transform.h"
#include "input.h"

namespace Engine {
    enum CameraMode {
        Free,
        Orbit
    };

    static std::string_view camera_mode_to_string_view(CameraMode camera_mode) {
        switch(camera_mode) {
            case Free: return "camera_mode_free";
            case Orbit: return "camera_mode_orbit";
            default: return "camera_mode_undefined";
        };
    }

    constexpr float DEFAULT_CAMERA_SPEED {10.f};

    class Camera : public Transform {
        private:
            float yaw {-90.f};
            float pitch {0.f};
            glm::mat4 projection;
        public:
            glm::vec3 position {glm::vec3(0, 0, 2)};
            glm::vec3 front;
            float speed { DEFAULT_CAMERA_SPEED };
            CameraMode mode;

        public:
            Camera(float width, float height, CameraMode mode = Free);
            void update(GLFWwindow* window, float delta_time);
            void refactor(float width, float height);

            glm::mat4 get_projection() { return projection; }
    };
}