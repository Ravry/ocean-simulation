#pragma once
#include <iostream>
#include <string_view>
#include <vector>
#include <map>
#include <memory>
#include "glad/glad.h"
#include "stb_image.h"

namespace Engine {
    class Texture {
    public:
        struct TextureCreateInfo {
            GLenum target;
            const char* file_path;
            unsigned int width;
            unsigned int height;
            std::map<unsigned int, std::string_view> layer_path_map;
            void* data_buffer;
        };

        Texture(const TextureCreateInfo& create_info);
        ~Texture();
        void bind(GLuint unit = 0);

    private:
        unsigned int id;
        GLenum target;
    };
}
