#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <implot.h>
#include "buffer.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "utils.h"

namespace Engine {
    namespace Game {
        class Renderer {
        public:
            Renderer(float width, float height);
            void update(GLFWwindow* window, float delta_time);
            void render();
            void refactor(int width, int height);
        private:
            void draw_imgui();

            std::unique_ptr<Camera> camera;
            std::map<std::string, Shader> shaders;
        };
    }
}