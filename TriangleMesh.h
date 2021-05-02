#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <glad/glad.h>

#include "Primitive.h"

class Renderer;
class TriangleMesh {
public:
    TriangleMesh(const std::vector<VertexData>& V, const std::vector<glm::ivec3>& ID);
    ~TriangleMesh();

    void Draw(const std::shared_ptr<Renderer>& renderer);

private:
    GLuint _VAO, _VBO, _EBO;
    int _numVertices, _numFaces;
};