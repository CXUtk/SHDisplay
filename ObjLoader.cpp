#include "ObjLoader.h"
#include <map>
#include <glm/gtx/transform.hpp>
#include "Primitive.h"


char ObjLoader::lineBuffer[MAX_BUFFER];
struct cmpVec3 {
    bool operator()(const glm::vec3& A, const glm::vec3& B) const {
        for (int i = 0; i < 3; i++) {
            if (A[i] != B[i]) {
                if (A[i] < B[i]) return true;
                else return false;
            }
        }
        return false;
    }
};
static std::map<glm::vec3, int, cmpVec3> pointSet;
static std::map<int, int> vIDMap;
void ObjLoader::load(const std::string& path, const std::string& prtPath) {
    Positions.clear();
    Triangles.clear();
    Normals.clear();
    TexCoords.clear();
    pointSet.clear();
    vIDMap.clear();
    _totV = 0;

    FILE* file = fopen(path.c_str(), "r");
    if (!file) {
        std::cerr << "Cannot open file " << path << std::endl;
        return;
    }
    while (fgets(lineBuffer, MAX_BUFFER, file)) {
        _ptr = 0;
        process();
    }
    fclose(file);

    FILE* prtfile = fopen(prtPath.c_str(), "r");
    if (!prtfile) {
        std::cerr << "Cannot open file " << path << std::endl;
        return;
    }
    
    int n;
    fscanf(prtfile, "%d", &n);
    if (n != Positions.size()) {
        std::cerr << "Size does not match!" << path << std::endl;
        return;
    }
    for(int j = 0; j < n; j++) {
        glm::mat3 prt{};
        for (int i = 0; i < 3; i++) {
            fscanf(prtfile, "%f %f %f", &prt[i].x, &prt[i].y, &prt[i].z);
        }
        PRTs.push_back(prt);
    }
    fclose(prtfile);
}

void ObjLoader::load(const std::string& path) {
    Positions.clear();
    Triangles.clear();
    pointSet.clear();
    vIDMap.clear();
    _totV = 0;

    FILE* file = fopen(path.c_str(), "r");
    if (!file) {
        std::cerr << "Cannot open file " << path << std::endl;
        return;
    }
    while (fgets(lineBuffer, MAX_BUFFER, file)) {
        _ptr = 0;
        process();
    }
    fclose(file);
}

std::shared_ptr<TriangleMesh> ObjLoader::GetMesh() const {

    std::vector<VertexData> Vertices;
    int sz = Positions.size();
    for (int i = 0; i < sz; i++) {
        VertexData v;
        memset(&v, 0, sizeof(v));
        v.Position = Positions[i];

        if (!Normals.empty()) {
            v.Normal = Normals[i];
        }
        if (!TexCoords.empty()) {
            v.TexCoords = TexCoords[i];
        }
        if (!PRTs.empty()) {
            v.PRT = PRTs[i];
        }
        Vertices.push_back(v);
    }
    std::vector<glm::ivec3> triangleFaceIndices;
    for (auto& t : Triangles) {
        triangleFaceIndices.push_back(glm::ivec3(t.VertexID[0], t.VertexID[1], t.VertexID[2]));
    }
    return std::make_shared<TriangleMesh>(Vertices, triangleFaceIndices);
}

bool readInt(const char* S, int& idx, int& num) {
    num = 0;
    while (S[idx] == ' ') idx++;
    bool neg = false;
    if (S[idx] == '-') {
        neg = true;
        idx++;
    }
    else if (S[idx] == '+') {
        idx++;
    }
    while (isdigit(S[idx])) {
        num *= 10;
        num += S[idx] - '0';
        idx++;
    }
    if (neg) num *= -1;
    return true;
}


//std::vector<DrawTriangle> ObjLoader::GetDrawTriangles() const {
//    std::vector<DrawTriangle> triangles;
//    for (auto face : Triangles) {
//        DrawTriangle tri;
//        DrawVertex v1[3];
//        for (int i = 0; i < 3; i++) {
//            v1[i].pos = Vertices[face.v[i]];
//            v1[i].normal = Normals[face.v[i]];
//            tri.V[i] = v1[i];
//        }
//
//        triangles.push_back(tri);
//    }
//    return triangles;
//}



void ObjLoader::process() {
    static char start[16];
    static char faceV[105];
    sscanf(lineBuffer + _ptr, "%s", start);
    _ptr += strlen(start);

    if (!strcmp(start, "v")) {
        double x, y, z;
        _ptr += sscanf(lineBuffer + _ptr, "%lf%lf%lf", &x, &y, &z);
        _totV++;
        auto pt = glm::vec3(x, y, z);
        Positions.push_back(pt);
    }
    else if (!strcmp(start, "f")) {
        int c = 0;
        std::vector<std::tuple<int, int, int>> vertices;
        while (~(c = sscanf(lineBuffer + _ptr, "%s", faceV))) {
            int id = 0;
            int vd[3];
            memset(vd, 0, sizeof(vd));
            for (int j = 0; j < 3; j++) {
                bool b = readInt(faceV, id, vd[j]);
                id++;
                if (!b || faceV[id - 1] != '/')break;
            }
            vertices.push_back({ vd[0] - 1, vd[1] - 1, vd[2] - 1 });
            _ptr += strlen(faceV) + 1;
        }

        // Triangulation process
        auto cmp = [&](int a, int b) {
            return Positions[a].x < Positions[b].x;
        };
        int sz = vertices.size();
        if (sz == 3) {
            Triangles.push_back(TriangleFaceIndex(vertices));
        }
        else {
            std::cerr << "Invalid obj file format" << std::endl;
            assert(false);
            return;
        }
    }
    else if (!strcmp(start, "vt")) {
        double x, y;
        _ptr += sscanf(lineBuffer + _ptr, "%lf%lf", &x, &y);
        TexCoords.push_back(glm::vec2(x, y));
    }
    else if (!strcmp(start, "vn")) {
        double x, y, z;
        _ptr += sscanf(lineBuffer + _ptr, "%lf%lf%lf", &x, &y, &z);
        Normals.push_back(glm::vec3(x, y, z));
    }
}
