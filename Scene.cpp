#include "Scene.h"
#include "ObjLoader.h"

char text[10005];
Scene::Scene(const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");

    int numModels;
    fscanf(file, "%d", &numModels);

    ObjLoader loader;
    for (int i = 0; i < numModels; i++) {
        fscanf(file, "%s", text);
        std::string p = "../../../resources/models/";
        p += text;

        glm::vec3 color;
        fscanf(file, "%f %f %f", &color.r, &color.g, &color.b);

        glm::mat4 transform;
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                fscanf(file, "%f", &transform[j][k]);
            }
        }

        loader.load(p.c_str(), file);
        auto mesh = loader.GetMesh(color, transform);
        _meshes.push_back(mesh);
    }

    fclose(file);
}

Scene::~Scene() {
}

void Scene::Draw(const std::shared_ptr<Renderer>& renderer, int prtID) {
    for (auto& model : _meshes) {
        model->Draw(renderer, prtID);
    }
}
