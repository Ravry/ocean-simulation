#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

namespace Engine {
    Texture::Texture(const TextureCreateInfo& create_info) : target(create_info.target) {
        glCreateTextures(target, 1, &id);

        switch (target) {
            case GL_TEXTURE_2D: {
                glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                int w, h, channels;
                unsigned char* data = stbi_load(create_info.file_path, &w, &h, &channels, 4);

                stbi_image_free(data);
                break;
            }
        }
    }

    void Texture::bind(GLuint unit) {
        glBindTextureUnit(unit, id);
    }

    Texture::~Texture() {
        glDeleteTextures(1, &id);
    }
}
