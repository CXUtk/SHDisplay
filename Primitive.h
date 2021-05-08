#pragma once
#include <glm/glm.hpp>

struct VertexData {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::mat3 PRT[3];
};
