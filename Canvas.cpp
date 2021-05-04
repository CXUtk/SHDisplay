#include "Canvas.h"
#include "ObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>
#include <filesystem>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


constexpr int WINDOW_WIDTH = 1024;
constexpr int WINDOW_HEIGHT = 768;

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
    _elapsedPerFrame = 1.0;
    double oldTime = glfwGetTime();
    while (!glfwWindowShouldClose(_window)) {
        beginIMGUI();
        update();
        draw();
        endIMGUI();

        auto now = glfwGetTime();
        _elapsedPerFrame = now - oldTime;
        oldTime = now;

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
    ImGui::ShowDemoWindow();
    ImGui::Begin("Info");
    ImGui::Text("FPS: %.2lf\n", 1.0 / _elapsedPerFrame);
    showSkyBoxSelector();
    showMeshModelSelector();

    ImGui::Checkbox("Gamma Correction", &_gammaCorrection);
    ImGui::Checkbox("Unshodowed", &_unShadowed);
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
        }
        if (ImGui::GetIO().MouseReleased[1]) {
            _isDragging = false;
        }

        // ¹öÂÖ¿ØÖÆÀëÔ­µã¾àÀë
        _factor += ImGui::GetIO().MouseWheel * -0.01f;
        _factor = std::max(0.f, std::min(1.0f, _factor));
        _distance = _factor * _factor * 20 + 0.5f;

    }
    float r = std::sin(_orbitParameter.y);
    glm::vec3 pos = glm::vec3(r * std::sin(-_orbitParameter.x), std::cos(_orbitParameter.y), r * std::cos(-_orbitParameter.x));
    _camera->SetEyePos(pos * _distance);
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
    _factor = 0.5f;
    _gammaCorrection = false;
    _unShadowed = false;
    _orbitParameter = glm::vec2(0, glm::half_pi<float>());
    _renderer = std::make_shared<Renderer>();

    _isDragging = false;
    _camera = std::make_shared<Camera>(glm::half_pi<float>(), (float)_width / _height, 0.2f, 100.f);
    _camera->SetEyePos(glm::vec3(0, 0, 5));
    
    ObjLoader loader;
    loader.load("../../../resources/scenes/bunny.obj", "../../../resources/scenes/bunny.prt");
    _meshMaps["Bunny"] = loader.GetMesh();
    loader.load("../../../resources/scenes/gd5k.obj", "../../../resources/scenes/gd5k.prt");
    _meshMaps["Sphere"] = loader.GetMesh();
    loader.load("../../../resources/scenes/gd32.obj", "../../../resources/scenes/gd32.prt");
    _meshMaps["Icosahedron"] = loader.GetMesh();
    loader.load("../../../resources/scenes/spot_triangulated_good.obj", "../../../resources/scenes/spot.prt");
    _meshMaps["Spot"] = loader.GetMesh();
    loader.load("../../../resources/scenes/dragon.obj", "../../../resources/scenes/dragon.prt");
    _meshMaps["Dragon"] = loader.GetMesh();


    std::filesystem::path path("../../../resources/prt/");
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        const auto filenameStr = entry.path().filename().string();
        if (entry.is_directory()) {
            auto dir = entry.path().string() + "/";
            _envirMaps[filenameStr] = std::make_shared<EnvironmentMap>(dir);
            printf("%s\n", dir.c_str());
        }
    }


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

void Canvas::showSkyBoxSelector() {
    static int style_idx = -1;
    std::vector<std::string> listNames;
    std::string str;
    for (auto& pair : _envirMaps) {
        str += pair.first;
        listNames.push_back(pair.first);
        str.push_back('\0');
    }
    if (style_idx == -1) {
        style_idx = 0;
        _renderer->SetEnvironment(_envirMaps[listNames[style_idx]]);
    }
    if (ImGui::Combo("Select SkyBox", &style_idx, str.c_str())) {
        _renderer->SetEnvironment(_envirMaps[listNames[style_idx]]);
    }
}

void Canvas::showMeshModelSelector() {
    static int model_idx = -1;
    std::vector<std::string> listNames;
    std::string str;
    for (auto& pair : _meshMaps) {
        str += pair.first;
        listNames.push_back(pair.first);
        str.push_back('\0');
    }
    if (model_idx == -1) {
        model_idx = 0;
        _mesh = _meshMaps[listNames[model_idx]];
    }
    if (ImGui::Combo("Select Model", &model_idx, str.c_str())) {
        _mesh = _meshMaps[listNames[model_idx]];
    }
}
