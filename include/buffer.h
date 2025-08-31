#pragma once
#include "glad/glad.h"
#include "texture.h"

namespace Engine {
    class GL_Object {
        protected:
            unsigned int id;
        public:
            unsigned int get_id() { return id; }
    };

    class Buffer : public GL_Object {
        public:
            Buffer();
            ~Buffer();
            void data(void* data, size_t data_size);
    };

    class VAO : public GL_Object {
        public:
            VAO();
            ~VAO();
            void bind();
            void bind_buffers(GLuint vbo_id, GLuint ebo_id);
            void attrib(GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint offset);
    };

    class FBO : public GL_Object {
        struct Attachment { 
            GLenum type;
            GLenum attachment;
            void* attachment_ptr;
        };

        private:
            std::vector<Attachment> attachments;
        public:
            FBO();
            ~FBO();
            void attach(GLenum attachment, Texture* texture);
            void bind(GLenum target = GL_FRAMEBUFFER);
            void refactor(unsigned int width, unsigned int height);
            static void unbind();
    };


    class SSBO : public GL_Object {
    public:
        SSBO();
        void data(unsigned int index, unsigned int *data, size_t data_size);
    };
}