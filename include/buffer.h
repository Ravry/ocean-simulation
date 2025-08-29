#pragma once
#include "glad/glad.h"

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

    class SSBO : public GL_Object {
    public:
        SSBO();
        void data(unsigned int index, unsigned int *data, size_t data_size);
    };
}