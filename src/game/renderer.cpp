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
        glEnable(GL_BLEND);  
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
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

    void ShowDockspace()
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("MainDockspace", nullptr, window_flags);
        ImGui::PopStyleVar(2);

        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");

        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        ImGui::End();
    }


    void Renderer::draw_imgui() {

        ImGui::Begin("viewport");
        {
            //TODO: draw framebuffer texture
        }
        ImGui::End();

        ImGui::Begin("miscellaneous");
        {
            if (ImGui::CollapsingHeader("information", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text(std::format("fps: {}", 1.f / Time::Timer::delta_time).c_str());
                ImGui::Text(std::format("eye: {}", camera_position_to_string_view(*camera).data()).c_str());
            }

            if (ImGui::CollapsingHeader("camera-settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::BeginCombo("camera-mode", camera_mode_to_string_view(camera->mode).data())) {
                    bool is_selected {false};
                    for (auto& camera_mode : {CameraMode::Free, CameraMode::Orbit}) {
                        if (ImGui::Selectable(camera_mode_to_string_view(camera_mode).data(), is_selected)) 
                            camera->mode = camera_mode;
                        if (is_selected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::InputFloat("camera-speed", &camera->speed, 1.f, 10.f);
            }

            if (ImGui::CollapsingHeader("ocean-settings", ImGuiTreeNodeFlags_DefaultOpen)) {}
        
            if (ImGui::CollapsingHeader("graph-preview", ImGuiTreeNodeFlags_DefaultOpen)) {
                constexpr double M_TAU = 2. * std::numbers::pi;
                
                constexpr size_t NUM_SAMPLES_SCATTER = 20;
                static float x_coords_scatter[NUM_SAMPLES_SCATTER] {};
                static float y_coords_scatter[NUM_SAMPLES_SCATTER] {};

                constexpr size_t NUM_SAMPLES_SIGNAL = 100;
                static float x_coords_signal[NUM_SAMPLES_SIGNAL] {};
                static float y_coords_signal[NUM_SAMPLES_SIGNAL] {};
                
                static bool once {false};
                if (!once) {
                    once = true;    
                    {
                        size_t j {0};
                        for (float i {0.f}; i < M_TAU; i += M_TAU / (NUM_SAMPLES_SCATTER)) {
                            std::complex<double> result = Utils::complex_exp(i);
                            x_coords_scatter[j] = result.real();
                            y_coords_scatter[j] = result.imag();
                            j++;
                        }

                        x_coords_scatter[j] = x_coords_scatter[0];
                        y_coords_scatter[j] = y_coords_scatter[0];
                    }
                    
                    {
                        size_t j {0};
                        for (float i {0.f}; i <= M_TAU; i += M_TAU/NUM_SAMPLES_SIGNAL) {
                            double signal_out = std::sin(i);
                            x_coords_signal[j] = i;
                            y_coords_signal[j] = signal_out;
                            j++;
                        }    
                    }
                }
                
                {
                    if (ImPlot::BeginPlot("fourier-visual", ImVec2(300, 300), ImPlotFlags_Equal)) {
                        ImPlot::PlotLine("point", x_coords_scatter, y_coords_scatter, NUM_SAMPLES_SCATTER + 1);
                        ImPlot::EndPlot();
                    }

                    ImGui::SameLine();

                    if (ImPlot::BeginPlot("signal", ImVec2(300, 300), ImPlotFlags_Equal)) {
                        ImPlot::PlotLine("line", x_coords_signal, y_coords_signal, NUM_SAMPLES_SIGNAL);
                        ImPlot::EndPlot();
                    }
                }
                
                ImGui::Spacing();

                {
                    if (ImPlot::BeginPlot("spectrum", ImVec2(607, 300), ImPlotFlags_Equal)) {
                        ImPlot::EndPlot();
                    }
                }

            }
        }
        ImGui::End();
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

        ShowDockspace();
        draw_imgui();
    }

    void Renderer::refactor(int width, int height) {
        glViewport(0, 0, width, height);
        camera->refactor(width, height);
    }
}