#include "TriangleMesh.h"
#include "Renderer.h"

TriangleMesh::TriangleMesh(const std::vector<VertexData>& V) {
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    _numVertices = V.size();
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, V.size() * sizeof(VertexData), V.data(), GL_STATIC_DRAW);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
    // TexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(6 * sizeof(float)));

    // PRT1
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(8 * sizeof(float)));
    // PRT2
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(11 * sizeof(float)));
    // PRT3
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(14 * sizeof(float)));

    for (int i = 0; i < 6; i++) glEnableVertexAttribArray(i);
    glBindVertexArray(0);
}

TriangleMesh::TriangleMesh(const std::vector<VertexData>& V, const std::vector<glm::ivec3>& ID) {
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    _numVertices = V.size();
    _numFaces = ID.size();
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, V.size() * sizeof(VertexData), V.data(), GL_STATIC_DRAW);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
    // TexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(6 * sizeof(float)));

    // PRT1
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(8 * sizeof(float)));
    // PRT2
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(11 * sizeof(float)));
    // PRT3
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(14 * sizeof(float)));

    for (int i = 0; i < 6; i++) glEnableVertexAttribArray(i);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * ID.size(), ID.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

TriangleMesh::~TriangleMesh() {
}

void TriangleMesh::Draw(const std::shared_ptr<Renderer>& renderer) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBindVertexArray(_VAO);
    //renderer->ApplyPhongShader(glm::vec3(0, 5, 5), glm::vec3(0.3, 0.5, 0.7));
    renderer->ApplyPRTShader();
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glDrawArrays(GL_TRIANGLES, 0, _numVertices);
    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
}
