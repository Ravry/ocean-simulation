#include "window.h"


namespace Engine {
    double Time::Timer::delta_time {0};

    static std::unique_ptr<Game::Renderer> renderer;

    Window::Window(int width, int height, std::string_view title)
    {
        if (!glfwInit()) return;

        window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
        glfwSetWindowPos(window, 200, 100);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSetKeyCallback(window, Input::key_callback);
        glfwSetMouseButtonCallback(window, Input::mouse_button_callback);
        glfwSetCursorPosCallback(window, Input::mouse_callback);
        glfwSetWindowSizeCallback(window, [] (GLFWwindow* window, int width, int height) { renderer->refactor(width, height); });

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
            ImGui::StyleColorsClassic();
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding    = 12.0f;
            style.ChildRounding     = 12.0f;
            style.PopupRounding     = 12.0f;
            style.FrameRounding     =  8.0f;
            style.ScrollbarRounding = 12.0f;
            style.GrabRounding      =  8.0f;
            style.FrameBorderSize   =  1.0f;
            style.WindowBorderSize  =  1.0f;
            style.PopupBorderSize   =  1.0f;
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 330 core");
            ImPlot::CreateContext();
        }

        renderer = std::make_unique<Game::Renderer>(width, height );
    }

    Window::~Window()
    {        
        ImPlot::DestroyContext();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::run()
    {
        while (!glfwWindowShouldClose(window)) {
            if (Input::is_key_pressed(GLFW_KEY_C)) glfwSetWindowShouldClose(window, GLFW_TRUE);

            static double last_time = glfwGetTime();
            double time = glfwGetTime();
            Time::Timer::delta_time = time - last_time;
            last_time = time;

            Input::update();
            glfwPollEvents();

            renderer->update(window, Time::Timer::delta_time);
            
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            renderer->render();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
            
            glfwSwapBuffers(window);
        }
    }
}