#pragma once
#include <glm/glm.hpp>
#include <string>
#include <cstdio>
#include <memory>

#include "Renderer.h"
#include "TriangleMesh.h"

class Scene {
public:
    Scene(const std::string& path);
    ~Scene();

    void Draw(const std::shared_ptr<Renderer>& renderer, int prtID);

private:
    std::vector<std::shared_ptr<TriangleMesh>> _meshes;
};
