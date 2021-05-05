#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <map>

#include "Renderer.h"
#include "TriangleMesh.h"
#include "Camera.h"

class Canvas {
public:
    static Canvas& GetInstance();
    ~Canvas();

    bool GetGammaCorrection() const { return _gammaCorrection; }
    bool IsUnshadowed() const { return _unShadowed; }
    glm::mat4 GetSkyBoxRotation() const;

    void Run();
private:
    // Singleton constructor
    Canvas(int width, int height);

    void update();
    void draw();
    void init();

    void beginIMGUI();
    void endIMGUI();

    void showSkyBoxSelector();
    void showMeshModelSelector();

    int _width, _height;
    GLFWwindow* _window;

    glm::vec2 _orbitParameter;
    bool _isDragging;
    float _factor;
    float _distance;

    double _elapsedPerFrame;

    bool _gammaCorrection;
    bool _unShadowed;

    glm::vec3 _skyBoxRotate;

    std::shared_ptr<Renderer> _renderer;
    std::shared_ptr<Camera> _camera;
    std::shared_ptr<TriangleMesh> _mesh;

    std::map<std::string, std::shared_ptr<EnvironmentMap>> _envirMaps;
    std::map<std::string, std::shared_ptr<TriangleMesh>> _meshMaps;
};
