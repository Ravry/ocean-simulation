#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

namespace Engine {
    Texture::Texture(const TextureCreateInfo& create_info) : target(create_info.target) {
        glCreateTextures(target, 1, &id);

        switch (target) {
            case GL_TEXTURE_2D: {
                glTextureParameteri(id, GL_TEXTURE_WRAP_S, create_info.wrap);
                glTextureParameteri(id, GL_TEXTURE_WRAP_T, create_info.wrap);
                glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, create_info.filter);
                glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, create_info.filter);

                glTextureStorage2D(id, 1, create_info.format, create_info.width, create_info.height);

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
