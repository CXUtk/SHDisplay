#include "Canvas.h"
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

glm::mat4 Canvas::GetSkyBoxRotation() const {
    return glm::rotate(_skyBoxRotate[0], glm::vec3(1, 0, 0))
        * glm::rotate(_skyBoxRotate[1], glm::vec3(0, 1, 0))
        * glm::rotate(_skyBoxRotate[2], glm::vec3(0, 0, 1));
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
    showPRTModeSelector();

    ImGui::Checkbox("Gamma Correction", &_gammaCorrection);
    //ImGui::Checkbox("Unshodowed", &_unShadowed);
    ImGui::SliderFloat("Skybox Rotation X", &_skyBoxRotate[0], -glm::pi<float>(), glm::pi<float>(), "%.2f");
    ImGui::SliderFloat("Skybox Rotation Y", &_skyBoxRotate[1], -glm::pi<float>(), glm::pi<float>(), "%.2f");
    ImGui::SliderFloat("Skybox Rotation Z", &_skyBoxRotate[2], -glm::pi<float>(), glm::pi<float>(), "%.2f");
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


        // 滚轮控制离原点距离
        _factor += ImGui::GetIO().MouseWheel * -0.01f;
        _factor = std::max(0.f, std::min(1.0f, _factor));
        _distance = _factor * _factor * 20 + 0.5f;
    }
    if (ImGui::GetIO().MouseReleased[1]) {
        _isDragging = false;
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
        _curScene->Draw(_renderer, (_prtMode == 2));
    }
    _renderer->End();
}

void Canvas::init() {

    _factor = 0.5f;
    _gammaCorrection = false;
    _prtMode = 0;
    _orbitParameter = glm::vec2(0, glm::half_pi<float>());
    _renderer = std::make_shared<Renderer>();
    _skyBoxRotate = glm::vec3(0);

    _isDragging = false;
    _camera = std::make_shared<Camera>(glm::half_pi<float>(), (float)_width / _height, 0.2f, 100.f);
    _camera->SetEyePos(glm::vec3(0, 0, 5));

    //ObjLoader loader;
    //loader.load("../../../resources/models/bunny.obj", "../../../resources/models/bunny.prt");
    //_meshMaps["Bunny"] = loader.GetMesh();
    //loader.load("../../../resources/models/gd5k.obj", "../../../resources/models/gd5k.prt");
    //_meshMaps["Sphere"] = loader.GetMesh();
    //loader.load("../../../resources/models/gd32.obj", "../../../resources/models/gd32.prt");
    //_meshMaps["Icosahedron"] = loader.GetMesh();
    //loader.load("../../../resources/models/spot_triangulated_good.obj", "../../../resources/models/spot.prt");
    //_meshMaps["Spot"] = loader.GetMesh();
    //loader.load("../../../resources/scenes/dragon.obj", "../../../resources/scenes/dragon.prt");
    //_meshMaps["Dragon"] = loader.GetMesh();

    _sceneMaps["Spot"] = std::make_shared<Scene>("../../../resources/scenes/spot.scene");
    _sceneMaps["Bunny"] = std::make_shared<Scene>("../../../resources/scenes/bunny.scene");
    _sceneMaps["Icosahedron"] = std::make_shared<Scene>("../../../resources/scenes/icosahedron.scene");
    _sceneMaps["Sphere2"] = std::make_shared<Scene>("../../../resources/scenes/sphere2.scene");
    _sceneMaps["Sphere"] = std::make_shared<Scene>("../../../resources/scenes/sphere.scene");

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
    for (auto& pair : _sceneMaps) {
        str += pair.first;
        listNames.push_back(pair.first);
        str.push_back('\0');
    }
    if (model_idx == -1) {
        model_idx = 0;
        _curScene = _sceneMaps[listNames[model_idx]];
    }
    if (ImGui::Combo("Select Scene", &model_idx, str.c_str())) {
        _curScene = _sceneMaps[listNames[model_idx]];
    }
}

void Canvas::showPRTModeSelector() {
    const char* text = "UnShodowed\0Shadowed\0Inter-Reflection\0";
    if (ImGui::Combo("PRT Mode", &_prtMode, text)) {
    }
}
