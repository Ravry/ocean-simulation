#pragma once
#include <string_view>
#include <vector>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "input.h"
#include "renderer.h"

namespace Engine {
    class Window {
    public:
        static Window& create_window(int width, int height, std::string_view title) {
            static Window instance(width, height, title);
            return instance;
        }
        
        Window(int width, int height, std::string_view title);
        ~Window();
        void run();

    private:
        GLFWwindow* window;
    };
}