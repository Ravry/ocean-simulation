#include "camera.h"

namespace Engine {
    Camera::Camera(float width, float height, CameraMode mode, float fov, float z_near, float z_far) 
    : Transform(glm::vec3(0), glm::vec3(0), glm::vec3(1)), mode(mode), fov(fov), z_near(z_near), z_far(z_far) 
    {
        projection = glm::perspective(glm::radians(fov), width/height, z_near, z_far);
    }

    static bool mode_change_pending { false };
    void Camera::set_mode(CameraMode camera_mode) {
        if (camera_mode == CameraMode::Free) mode_change_pending = true;
        mode = camera_mode;
    }

    void Camera::update(GLFWwindow* window, float delta_time) {
        static bool cursor_enabled {true};
        
        if (Input::is_key_pressed(GLFW_KEY_ESCAPE)) {
            cursor_enabled = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        if (Input::is_key_pressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            cursor_enabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        switch (mode) {
            case Free: {       
                if (cursor_enabled && !mode_change_pending) return;
                mode_change_pending = false;
                
                glm::vec3 input = glm::vec3(0.f);
                if (Input::is_key_held_down(GLFW_KEY_W)) input.z += 1; 
                if (Input::is_key_held_down(GLFW_KEY_S)) input.z -= 1; 
                if (Input::is_key_held_down(GLFW_KEY_A)) input.x -= 1; 
                if (Input::is_key_held_down(GLFW_KEY_D)) input.x += 1; 
                if (Input::is_key_held_down(GLFW_KEY_SPACE)) input.y += 1; 
                if (Input::is_key_held_down(GLFW_KEY_LEFT_CONTROL)) input.y -= 1; 

                if (glm::length(input) > 0) input = glm::normalize(input);

                yaw += Input::delta_x;
                pitch -= Input::delta_y;
                pitch = std::clamp(pitch, -80.f, 80.f);

                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                glm::vec3 cameraFront = glm::normalize(front);
                glm::vec3 cameraUp = glm::vec3(0, 1, 0);
                glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

                position +=  (float)delta_time * speed * (input.z * cameraFront + input.y * cameraUp + input.x * cameraRight);
                matrix = glm::lookAt(position, position + cameraFront, cameraUp);
                break;
            }
            case Orbit: {
                double time {glfwGetTime()};
                const float amplitude {18.f};
                float frequency {speed / (DEFAULT_CAMERA_SPEED * 2.f)};
                position = glm::vec3(amplitude * cos(time * frequency), 14.f, amplitude * sin(time * frequency));
                matrix = glm::lookAt(position, glm::vec3(0.f, -7.f, 0.f), glm::vec3(0.f, 1.f, 0.f));    
                break;
            }
        }
    }

    void Camera::refactor(float width, float height) {
        projection = glm::perspective(glm::radians(fov), width/height, z_near, z_far);
    }
}