#include "TriangleMesh.h"
#include "Renderer.h"

void appendMat3(int& start, int& offset) {
    // PRT1
    glVertexAttribPointer(start, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(offset * sizeof(float)));
    // PRT2
    glVertexAttribPointer(start + 1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)((offset + 3) * sizeof(float)));
    // PRT3
    glVertexAttribPointer(start + 2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)((offset + 6) * sizeof(float)));

    start += 3;
    offset += 9;
}

TriangleMesh::TriangleMesh(const std::vector<VertexData>& V, const std::vector<VertexData>& V_IR, glm::vec3 color) :
    _color(color) {
    glGenVertexArrays(2, _VAO);
    glGenBuffers(2, _VBO);

    _numVertices = V.size();

    for (int i = 0; i < 2; i++) {
        glBindVertexArray(_VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, V.size() * sizeof(VertexData), i ? V_IR.data() : V.data(), GL_STATIC_DRAW);
        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
        // Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
        // TexCoord
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(6 * sizeof(float)));


        int start = 3, offset = 8;
        // PRTs
        appendMat3(start, offset);
        appendMat3(start, offset);
        appendMat3(start, offset);

        //// PRT1
        //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(8 * sizeof(float)));
        //// PRT2
        //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(11 * sizeof(float)));
        //// PRT3
        //glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(14 * sizeof(float)));

        for (int j = 0; j < start; j++) glEnableVertexAttribArray(j);
        glBindVertexArray(0);
    }
}
//
//TriangleMesh::TriangleMesh(const std::vector<VertexData>& V, const std::vector<glm::ivec3>& ID) {
//    glGenVertexArrays(1, &_VAO);
//    glGenBuffers(1, &_VBO);
//    glGenBuffers(1, &_EBO);
//
//    _numVertices = V.size();
//    _numFaces = ID.size();
//    glBindVertexArray(_VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
//    glBufferData(GL_ARRAY_BUFFER, V.size() * sizeof(VertexData), V.data(), GL_STATIC_DRAW);
//    // Position
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
//    // Normal
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
//    // TexCoord
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(6 * sizeof(float)));
//
//    // PRT1
//    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(8 * sizeof(float)));
//    // PRT2
//    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(11 * sizeof(float)));
//    // PRT3
//    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(14 * sizeof(float)));
//
//    for (int i = 0; i < 6; i++) glEnableVertexAttribArray(i);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * ID.size(), ID.data(), GL_STATIC_DRAW);
//
//    glBindVertexArray(0);
//}

TriangleMesh::~TriangleMesh() {
}

void TriangleMesh::Draw(const std::shared_ptr<Renderer>& renderer, int id) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBindVertexArray(_VAO[id]);
    //renderer->ApplyPhongShader(glm::vec3(0, 5, 5), glm::vec3(0.3, 0.5, 0.7));
    renderer->ApplyPRTShader(_color);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glDrawArrays(GL_TRIANGLES, 0, _numVertices);
    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
}
