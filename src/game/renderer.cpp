#include "renderer.h"

namespace Engine::Game {
    std::unique_ptr<Texture> texture_framebuffer_color;
    std::unique_ptr<Texture> texture_framebuffer_depth;
    std::unique_ptr<FBO> framebuffer;
    std::unique_ptr<VAO> vao;
    std::unique_ptr<Buffer> vbo;
    std::unique_ptr<Buffer> ebo;
    Mesh mesh_plane;

    Renderer::Renderer(float width, float height) {        
        //SHADER-INIT
        {
            shaders["default"] = Shader(
                ASSETS_DIR "shaders/default/vert.glsl",
                ASSETS_DIR "shaders/default/frag.glsl"
            );

            shaders["ocean"] = Shader(
                ASSETS_DIR "shaders/ocean/vert.glsl",
                ASSETS_DIR "shaders/ocean/frag.glsl"
            );
        }

        //FRAMEBUFFER-INIT
        {
            framebuffer = std::make_unique<FBO>();
        
            {
                Texture::TextureCreateInfo create_info {GL_TEXTURE_2D};
                create_info.width = width;
                create_info.height = height;
                create_info.format = GL_RGB8;
                create_info.filter = GL_LINEAR;
                create_info.wrap = GL_CLAMP_TO_EDGE;
                texture_framebuffer_color = std::make_unique<Texture>(create_info);
            }

            {
                Texture::TextureCreateInfo create_info {GL_TEXTURE_2D};
                create_info.width = width;
                create_info.height = height;
                create_info.format = GL_DEPTH_COMPONENT24;
                create_info.filter = GL_LINEAR;
                create_info.wrap = GL_CLAMP_TO_EDGE;
                texture_framebuffer_depth = std::make_unique<Texture>(create_info);  
            }

            framebuffer->attach(GL_COLOR_ATTACHMENT0, texture_framebuffer_color.get());
            framebuffer->attach(GL_DEPTH_ATTACHMENT, texture_framebuffer_depth.get());
            framebuffer->set_draw_buffers({ GL_COLOR_ATTACHMENT0 });
            framebuffer->status();
        }
        
        //PLANE-INIT
        {

            constexpr size_t SIZE { 20 };
            constexpr size_t HALF_SIZE { SIZE / 2 };
            constexpr size_t RESOLUTION { 4 };
            constexpr size_t REAL_SIZE { SIZE * RESOLUTION };
                     
            for (size_t z {0}; z <= REAL_SIZE; z++) {
                for (size_t x {0}; x <= REAL_SIZE; x++) {
                    float _x = static_cast<float>(x)/RESOLUTION;
                    float _z = static_cast<float>(z)/RESOLUTION;
                    
                    mesh_plane.vertices.push_back(Vertex {
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

                    mesh_plane.indices.push_back(topLeft);
                    mesh_plane.indices.push_back(bottomLeft);
                    mesh_plane.indices.push_back(topRight);

                    mesh_plane.indices.push_back(topRight);
                    mesh_plane.indices.push_back(bottomLeft);
                    mesh_plane.indices.push_back(bottomRight);;
                }
            }

            vao = std::make_unique<VAO>();
            vbo = std::make_unique<Buffer>();
            ebo = std::make_unique<Buffer>();
        
            vbo->data(mesh_plane.vertices.data(), mesh_plane.vertices.size() * sizeof(Vertex));
            ebo->data(mesh_plane.indices.data(), mesh_plane.indices.size() * sizeof(uint32_t));
            
            vao->attrib(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            vao->bind_buffers(vbo->get_id(), ebo->get_id());            
        }

        //GL-INIT
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);  
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
            glClearColor(.12f, .12f, .12f, 1.f);
            glViewport(0, 0, width, height);
        }
    
        //ENGINE-INIT
        {
            camera = std::make_unique<Camera>(width, height, CameraMode::Orbit, 70.f);
        }
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

    void draw_imgui_information_header(Camera* camera) {
        if (ImGui::CollapsingHeader("information", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text(std::format("fps: {}", 1.f / Time::Timer::delta_time).c_str());
            ImGui::Text(std::format("eye: {}", camera_position_to_string_view(camera).data()).c_str());
        }
    }

    void draw_imgui_camera_settings_header(Camera* camera) {
        if (ImGui::CollapsingHeader("camera-settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::BeginCombo("camera-mode", camera_mode_to_string_view(camera->mode).data())) {
                bool is_selected {false};
                for (auto& camera_mode : {CameraMode::Free, CameraMode::Orbit}) {
                    if (ImGui::Selectable(camera_mode_to_string_view(camera_mode).data(), is_selected)) 
                        camera->set_mode(camera_mode);
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::InputFloat("camera-speed", &camera->speed, 1.f, 10.f);
        }

    }

    void draw_imgui_ocean_settings_header() {
        if (ImGui::CollapsingHeader("ocean-settings", ImGuiTreeNodeFlags_DefaultOpen)) {}
    }
    
    void draw_imgui_graph_preview_header() {
        static double timer = 0.f;
        timer += Time::Timer::delta_time;

        constexpr double M_TAU = 2. * std::numbers::pi;

        constexpr double SIGNAL_TOTAL_DURATION = M_TAU;
        constexpr size_t NUM_SAMPLES_SIGNAL = 100;
        
        static double x_coords_signal[NUM_SAMPLES_SIGNAL + 1] {};
        static double y_coords_signal[NUM_SAMPLES_SIGNAL + 1] {};

        static double x_coords_scatter[NUM_SAMPLES_SIGNAL + 1] {};
        static double y_coords_scatter[NUM_SAMPLES_SIGNAL + 1] {};

        static double frequency_spectrum[(NUM_SAMPLES_SIGNAL/2) + 1] {};
        static std::complex<double> complex_engergy_spectrum[(NUM_SAMPLES_SIGNAL/2) + 1] {};
        static double energy_spectrum[(NUM_SAMPLES_SIGNAL/2) + 1] {};

        static double x_coords_ift_signal[NUM_SAMPLES_SIGNAL + 1] {};
        static double y_coords_ift_signal[NUM_SAMPLES_SIGNAL + 1] {}; 
        
        {
            static bool once {false};
            if (!once) {
                {
                    for (size_t i {0}; i <= NUM_SAMPLES_SIGNAL; i ++) {
                        double signal_in = ((double)i/NUM_SAMPLES_SIGNAL) * SIGNAL_TOTAL_DURATION;
                        double signal_out = 
                            1.f * std::sin( 1.f * signal_in) + 
                            .2f * std::sin( 5.f * signal_in) + 
                            .5f * std::sin(10.f * signal_in) + 
                           .75f * std::cos( 7.f * signal_in);
                        x_coords_signal[i] = signal_in;
                        y_coords_signal[i] = signal_out;
                    }    
                }
                once = true;                    
            }
        }

        constexpr float DELTA_TIME_FOR_DESIRED_FPS = 1.f/20.f;
        constexpr double sampling_rate = NUM_SAMPLES_SIGNAL / SIGNAL_TOTAL_DURATION;
        constexpr double nyquist_frequency = sampling_rate / 2.0;
        static size_t j {0};
        
        static double x_coord_center_of_mass[1];
        static double y_coord_center_of_mass[1];

        if (timer >= DELTA_TIME_FOR_DESIRED_FPS)
        {
            static bool ift_done {false};
            timer = 0;
            if (j <= NUM_SAMPLES_SIGNAL/2) {
                double frequency = (double)j * (sampling_rate / NUM_SAMPLES_SIGNAL);
                std::complex<double> sum(0, 0); 
                
                for (size_t i {0}; i <= NUM_SAMPLES_SIGNAL; i++) {
                    double input = ((double)i/NUM_SAMPLES_SIGNAL) * SIGNAL_TOTAL_DURATION;
                    std::complex<double> result = y_coords_signal[i] * Utils::complex_exp(-frequency, input);
                    sum += result;
                    x_coords_scatter[i] = result.real();
                    y_coords_scatter[i] = result.imag();
                }
                
                x_coord_center_of_mass[0] = sum.real() / (NUM_SAMPLES_SIGNAL + 1);
                y_coord_center_of_mass[0] = sum.imag() / (NUM_SAMPLES_SIGNAL + 1);
                
                double magnitude = std::abs(sum);

                frequency_spectrum[j] = frequency;
                complex_engergy_spectrum[j] = sum / static_cast<double>(NUM_SAMPLES_SIGNAL + 1);
                energy_spectrum[j] = magnitude / (NUM_SAMPLES_SIGNAL + 1);
                j++;
            }
            else if (!ift_done) {
                ift_done = true;
                for (size_t i {0}; i <= NUM_SAMPLES_SIGNAL; i++) {
                    double signal_in = ((double)i/NUM_SAMPLES_SIGNAL) * SIGNAL_TOTAL_DURATION;
                    std::complex<double> sum_complex(0, 0);
                    for (size_t k {0}; k <= NUM_SAMPLES_SIGNAL/2; k++) {
                        double frequency = frequency_spectrum[k];
                        std::complex<double> coefficient = complex_engergy_spectrum[k];
                        double weight = (k == 0 || k == NUM_SAMPLES_SIGNAL/2) ? 1.0 : 2.0;
                        sum_complex += weight * coefficient * Utils::complex_exp(frequency, signal_in);
                    }
                    
                    x_coords_ift_signal[i] = signal_in;
                    y_coords_ift_signal[i] = sum_complex.real();
                }
            }
        }
            
        if (ImGui::CollapsingHeader("graph-preview", ImGuiTreeNodeFlags_DefaultOpen)) {
            {
                if (ImPlot::BeginPlot("forward-fourier-transform", ImVec2(300, 300), ImPlotFlags_Equal)) {
                    ImPlot::PlotLine("line", x_coords_scatter, y_coords_scatter, NUM_SAMPLES_SIGNAL + 1);
                    ImPlot::PlotScatter("scatter", x_coord_center_of_mass, y_coord_center_of_mass, 1);
                    ImPlot::EndPlot();
                }

                ImGui::SameLine();

                if (ImPlot::BeginPlot("signal", ImVec2(300, 300), ImPlotFlags_Equal)) {
                    ImPlot::PlotLine("line", x_coords_signal, y_coords_signal, NUM_SAMPLES_SIGNAL + 1);
                    ImPlot::EndPlot();
                }
            }
            
            ImGui::Spacing();

            {
                if (ImPlot::BeginPlot("frequency-domain-spectrum (dft)", ImVec2(607, 200), ImPlotFlags_Equal)) {
                    ImPlot::PlotLine("line", frequency_spectrum, energy_spectrum, (NUM_SAMPLES_SIGNAL/2) + 1);
                    ImPlot::EndPlot();
                }

                if (ImPlot::BeginPlot("time-domain-spectrum (ift)", ImVec2(607, 200), ImPlotFlags_Equal)) {
                    ImPlot::PlotLine("line", x_coords_ift_signal, y_coords_ift_signal, NUM_SAMPLES_SIGNAL + 1);
                    ImPlot::EndPlot();
                }
            }
        }
    }

    void Renderer::draw_imgui() 
    {
        {
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
            
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            
            ImGuiWindowFlags window_flags = 
                ImGuiWindowFlags_NoDocking  |   ImGuiWindowFlags_NoTitleBar             | 
                ImGuiWindowFlags_NoCollapse |   ImGuiWindowFlags_NoResize               | 
                ImGuiWindowFlags_NoMove     |   ImGuiWindowFlags_NoBringToFrontOnFocus  | 
                ImGuiWindowFlags_NoNavFocus;

            ImGui::Begin("MainDockspace", nullptr, window_flags);
            ImGui::PopStyleVar(3);

            ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
            
            static bool once {false};
            if (!once) {
                once = true;
                
                ImGui::DockBuilderRemoveNode(dockspace_id); 
                ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

                ImGuiID dock_id_left, dock_id_right;
                ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, .41f, &dock_id_left, &dock_id_right);

                ImGui::DockBuilderDockWindow("viewport", dock_id_right);
                ImGui::DockBuilderDockWindow("miscellaneous", dock_id_left);
                
                ImGui::DockBuilderFinish(dockspace_id);
            }

            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            ImGui::End();
        }
     
        {
            {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);         
                ImVec2 current_size;
                ImGui::Begin("viewport");
                {
                    current_size = ImGui::GetContentRegionAvail();
                    ImGui::Image((void*)(intptr_t)texture_framebuffer_color->get_id(), current_size, ImVec2(0, 1), ImVec2(1, 0));
                }
                static ImVec2 last_size { ImVec2(0, 0) };
                static bool once { false };
                static bool resizing { false };
                bool size_changed = current_size.x != last_size.x || current_size.y != last_size.y;

                if (size_changed) {
                    last_size = current_size;
                    resizing = true;
                }
                else if (resizing && (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || !once)) {
                    once = true;
                    refactor(current_size.x, current_size.y);
                    resizing = false;
                }

                ImGui::End();
                ImGui::PopStyleVar(3);
            }
            
            {
                ImGui::Begin("miscellaneous");
                {
                    draw_imgui_information_header(camera.get());
                    draw_imgui_camera_settings_header(camera.get());
                    draw_imgui_ocean_settings_header();
                    draw_imgui_graph_preview_header();
                }
                ImGui::End();
            }
        }
    }

    void Renderer::render() {
        framebuffer->bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders["ocean"]
            .set_uniform_mat4("model", glm::mat4(1.f))
            .set_uniform_mat4("view", camera->get_matrix())
            .set_uniform_mat4("projection", camera->get_projection())
            .set_uniform_float("time", glfwGetTime())
            .use();
        vao->bind();
        glDrawElements(GL_TRIANGLES, mesh_plane.indices.size(), GL_UNSIGNED_INT, 0);
        Shader::unuse();

        FBO::unbind();

        draw_imgui();
    }

    void Renderer::refactor(int width, int height) {
        out("refactor: (width={}; height={})", width, height);
        if (width <= 0 || height <= 0) return;
        glViewport(0, 0, width, height);
        framebuffer->refactor(width, height);
        camera->refactor(width, height);
    }
}