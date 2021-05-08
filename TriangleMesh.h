#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <glad/glad.h>

#include "Primitive.h"

class Renderer;
class TriangleMesh {
public:
    TriangleMesh(const std::vector<VertexData>& V, const std::vector<VertexData>& V_IR, glm::vec3 color);
    //TriangleMesh(const std::vector<VertexData>& V, const std::vector<glm::ivec3>& ID);
    ~TriangleMesh();

    void Draw(const std::shared_ptr<Renderer>& renderer, int id);

private:
    GLuint _VAO[2], _VBO[2];
    int _numVertices, _numFaces;

    glm::vec3 _color;
};
