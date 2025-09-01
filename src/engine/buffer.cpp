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

    void FBO::attach(GLenum attachment, Texture* texture) {
        glNamedFramebufferTexture(id, attachment, texture->get_id(), 0);
        attachments.push_back(Attachment {
            .type = GL_TEXTURE,
            .attachment = attachment, 
            .attachment_ptr = reinterpret_cast<void*>(texture)
        });
    }

    void FBO::bind(GLenum target) { 
        glBindFramebuffer(target, id);
    }

    void FBO::refactor(unsigned int width, unsigned int height) {
        for (const auto& attachment : attachments) {
            if (attachment.type == GL_TEXTURE) {
                Texture* texture_attachment = reinterpret_cast<Texture*>(attachment.attachment_ptr);
                texture_attachment->refactor(width, height);
                attach(attachment.attachment, texture_attachment);
            }
        }
    }

    void FBO::status() {
        GLenum status = glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer not complete! Status: " << status << std::endl;
            switch (status) {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    std::cerr << "Incomplete attachment" << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    std::cerr << "Missing attachment" << std::endl;
                    break;
                case GL_FRAMEBUFFER_UNSUPPORTED:
                    std::cerr << "Unsupported framebuffer format" << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                    std::cerr << "Incomplete multisample" << std::endl;
                    break;
            }
        }
    }

    void FBO::set_draw_buffers(const std::vector<GLenum>& buffers) {
        glNamedFramebufferDrawBuffers(id, static_cast<GLsizei>(buffers.size()), buffers.data());
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