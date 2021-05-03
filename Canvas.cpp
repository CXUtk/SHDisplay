#include "Canvas.h"
#include "ObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


Canvas& Canvas::GetInstance() {
    static Canvas canvas(WINDOW_WIDTH, WINDOW_HEIGHT);
    return canvas;
}

Canvas::~Canvas() {
    _renderer.reset();
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Canvas::Run() {
    while (!glfwWindowShouldClose(_window)) {
        beginIMGUI();
        update();
        draw();
        endIMGUI();

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

Canvas::Canvas(int width, int height) : _width(width), _height(height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    
    _window = glfwCreateWindow(width, height, "PRT Lighting", nullptr, nullptr);
    if (!_window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load glad!\n");
        glfwTerminate();
        return;
    }
    glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    init();
}

void Canvas::update() {
    ImGui::Begin("Info");
    ImGui::Text("FPS: %.lf\n", 1 / 0.5);
    ImGui::End();


    if (!ImGui::GetIO().WantCaptureMouse) {
        if (ImGui::GetIO().MouseClicked[1]) {
            _isDragging = true;
        }
        if (_isDragging) {
            _orbitParameter += 0.005f * glm::vec2(ImGui::GetIO().MouseDelta.x, -ImGui::GetIO().MouseDelta.y);
            auto pi = glm::pi<float>();
            _orbitParameter.x = std::max(-pi, std::min(pi, _orbitParameter.x));
            _orbitParameter.y = std::max(0.001f, std::min(pi - 0.001f, _orbitParameter.y));

            float r = std::sin(_orbitParameter.y);
            glm::vec3 pos = glm::vec3(r * std::sin(-_orbitParameter.x), std::cos(_orbitParameter.y), r * std::cos(-_orbitParameter.x));
            _camera->SetEyePos(pos * 5.f);
        }
        if (ImGui::GetIO().MouseReleased[1]) {
            _isDragging = false;
        }

        //// ¹öÂÖ¿ØÖÆÀëÔ­µã¾àÀë
        //if (input->getScrollValue() != 0) {
        //    _factor += input->getScrollValue() * -0.01f;
        //    _factor = std::max(0.f, std::min(1.0f, _factor));
        //    _distance = _factor * _factor * 20 + 0.5f;
        //}

    }
}

void Canvas::draw() {
    auto model = glm::identity<glm::mat4>();

    _renderer->ClearCurrentFrame();
    _renderer->Begin(_camera->GetProjectionMatrix(), _camera->GetViewMatrix(), model);
    {
        _renderer->DrawSkyBox();
        _mesh->Draw(_renderer);
    }
    _renderer->End();
}

void Canvas::init() {
    _orbitParameter = glm::vec2(0, glm::half_pi<float>());
    _renderer = std::make_shared<Renderer>();

    _isDragging = false;
    _camera = std::make_shared<Camera>(glm::half_pi<float>(), (float)_width / _height, 0.5f, 100.f);
    _camera->SetEyePos(glm::vec3(0, 0, 5));
    
    ObjLoader loader;
    loader.load("../../../resources/scenes/bunny.obj", "../../../resources/scenes/bunny.prt");
    _mesh = loader.GetMesh();


    _renderer->SetEnvironment(std::make_shared<EnvironmentMap>("../../../resources/prt/SkyBox/"));


    // Initialize IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Canvas::beginIMGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Canvas::endIMGUI() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
