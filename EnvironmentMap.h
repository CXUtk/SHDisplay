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
    std::vector<glm::mat3> GetLightFunction(const glm::mat4& rotation) const;
    std::vector<glm::mat4> GetUnshadowQuadraticForm(const glm::mat4& rotation) const;

private:
    GLuint _cubemapTexture;
    glm::mat3 _prt[3];

};