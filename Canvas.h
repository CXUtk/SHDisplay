#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Renderer.h"
#include "TriangleMesh.h"

class Canvas {
public:
    static Canvas& GetInstance();
    ~Canvas();

    void Run();
private:
    // Singleton constructor
    Canvas(int width, int height);

    void update();
    void draw();
    void init();


    int _width, _height;
    GLFWwindow* _window;

    std::shared_ptr<Renderer> _renderer;
    std::shared_ptr<TriangleMesh> _mesh;
};