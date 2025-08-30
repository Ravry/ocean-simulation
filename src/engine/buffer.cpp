#include "buffer.h"

namespace Engine {
    Buffer::Buffer() {
        glCreateBuffers(1, &id);
    }

    Buffer::~Buffer() {
        glDeleteBuffers(1, &id);
    }

    void Buffer::data(void* data, size_t data_size) {
        glNamedBufferData(id, data_size, data, GL_STATIC_DRAW);
    }

    VAO::VAO() {
        glCreateVertexArrays(1, &id);
    }

    VAO::~VAO() {
        glDeleteVertexArrays(1, &id);
    }

    void VAO::bind() {
        glBindVertexArray(id);
    }

    void VAO::bind_buffers(GLuint vbo_id, GLuint ebo_id) {
        glVertexArrayVertexBuffer(id, 0, vbo_id, 0, 3 * sizeof(float));
        glVertexArrayElementBuffer(id, ebo_id);
    }

    void VAO::attrib(GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint offset)
    {
        glEnableVertexArrayAttrib(id, index);
        glVertexArrayAttribFormat(id, index, size, type, normalized, offset);
    }

    FBO::FBO() {
        glCreateFramebuffers(1, &id);
    }

    FBO::~FBO() { 
        glDeleteFramebuffers(1, &id);
    }

    void FBO::attach(GLenum attachment, GLuint texture) {
        glNamedFramebufferTexture(id, attachment, texture, 0);
    }

    void FBO::bind(GLenum target) { 
        glBindFramebuffer(target, id);
    }

    void FBO::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    SSBO::SSBO() {
        glCreateBuffers(1, &id);
    }

    void SSBO::data(unsigned int index, unsigned int *data, size_t data_size) {
        glNamedBufferData(id, data_size, data, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, id);
    }
}