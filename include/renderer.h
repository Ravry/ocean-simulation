#pragma once
#include "buffer.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

namespace Engine {
    namespace Game {
        class Renderer {
        public:
            Renderer(float width, float height);
            void update(GLFWwindow* window, float delta_time);
            void render();
            void refactor(int width, int height);
        private:
            std::unique_ptr<Camera> camera;
            std::map<std::string, Shader> shaders;
        };
    }
}