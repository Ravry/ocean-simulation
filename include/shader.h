#pragma once
#include <string_view>
#include <filesystem>
#include <glad/glad.h>
#include "transform.h"
#include "utils.h"

namespace Engine {
    class Shader {
        private:
            unsigned int id;
            std::string_view vert_file;
            std::string_view frag_file;

        public:
            Shader() = default;
            Shader(std::string_view vertex_shader_file, std::string_view fragment_shader_file);
            ~Shader();
            void use();
            void reload();

            Shader& set_uniform_float(std::string_view name, float value);
            Shader& set_uniform_mat4(std::string_view name, glm::mat4 matrix);
            Shader& set_uniform_vec3(std::string_view name, glm::vec3 vector);

            static void unuse();

        private:
            void load(std::string_view vertex_shader_file, std::string_view fragment_shader_file);
    };
}