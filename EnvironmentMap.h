#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <glad/glad.h>

#include "Primitive.h"

class EnvironmentMap {
public:
    EnvironmentMap(const std::string& path);
    ~EnvironmentMap();

    GLuint GetTexture() const { return _cubemapTexture; }
    glm::mat3 GetTransferFunction(int index) const { return _prt[index]; }

private:
    GLuint _cubemapTexture;
    glm::mat3 _prt[3];
};