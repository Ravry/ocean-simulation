#include "renderer.h"

namespace Engine::Game {
    struct Vertex {
        glm::vec3 position;
    };

    std::unique_ptr<VAO> vao;
    std::unique_ptr<Buffer> vbo;
    std::unique_ptr<Buffer> ebo;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices; 

    Renderer::Renderer(float width, float height) {
        camera = std::make_unique<Camera>(width, height, CameraMode::Orbit);

        shaders["default"] = Shader(
            ASSETS_DIR "shaders/default/vert.glsl",
            ASSETS_DIR "shaders/default/frag.glsl"
        );

        shaders["ocean"] = Shader(
            ASSETS_DIR "shaders/ocean/vert.glsl",
            ASSETS_DIR "shaders/ocean/frag.glsl"
        );

        {
            constexpr size_t SIZE { 20 };
            constexpr size_t HALF_SIZE { SIZE / 2 };
            constexpr size_t RESOLUTION { 4 };
            constexpr size_t REAL_SIZE { SIZE * RESOLUTION };
                     
            for (size_t z {0}; z <= REAL_SIZE; z++) {
                for (size_t x {0}; x <= REAL_SIZE; x++) {
                    float _x = static_cast<float>(x)/RESOLUTION;
                    float _z = static_cast<float>(z)/RESOLUTION;
                    
                    vertices.push_back(Vertex {
                        glm::vec3{_x - HALF_SIZE, 0, _z - HALF_SIZE}
                    });
                }
            }

            for (size_t z {0}; z < REAL_SIZE; z++) {
                for (size_t x {0}; x < REAL_SIZE; x++) {
                    uint32_t topLeft     = z * (REAL_SIZE + 1) + x;
                    uint32_t topRight    = topLeft + 1;
                    uint32_t bottomLeft  = topLeft + (REAL_SIZE + 1);
                    uint32_t bottomRight = bottomLeft + 1;

                    indices.push_back(topLeft);
                    indices.push_back(bottomLeft);
                    indices.push_back(topRight);

                    indices.push_back(topRight);
                    indices.push_back(bottomLeft);
                    indices.push_back(bottomRight);;
                }
            }

            vao = std::make_unique<VAO>();
            vbo = std::make_unique<Buffer>();
            ebo = std::make_unique<Buffer>();
        
            vbo->data(vertices.data(), vertices.size() * sizeof(Vertex));
            ebo->data(indices.data(), indices.size() * sizeof(uint32_t));
            
            vao->attrib(0, 3, GL_FLOAT, GL_FALSE, 0);
            vao->bind_buffers(vbo->get_id(), ebo->get_id());            
        }
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glEnable(GL_DEPTH_TEST);
        glClearColor(.12f, .12f, .12f, 1.f);
    }
        
    void Renderer::update(GLFWwindow* window, float delta_time) {
        camera->update(window, delta_time);

        if (Input::is_key_pressed(GLFW_KEY_X)) {
            static bool show_polygon {false};
            show_polygon = !show_polygon;
            glPolygonMode(GL_FRONT_AND_BACK, show_polygon ? GL_LINE : GL_FILL);
        }

        if (Input::is_key_pressed(GLFW_KEY_R)) {
            for (auto& shader : shaders) {
                shader.second.reload();
            }
            out("shaders reloaded ...");
        }
    }

    void Renderer::render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders["ocean"]
            .set_uniform_mat4("model", glm::mat4(1.f))
            .set_uniform_mat4("view", camera->get_matrix())
            .set_uniform_mat4("projection", camera->get_projection())
            .set_uniform_float("time", glfwGetTime())
            .use();
        vao->bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        Shader::unuse();
    }

    void Renderer::refactor(int width, int height) {
        glViewport(0, 0, width, height);
        camera->refactor(width, height);
    }
}