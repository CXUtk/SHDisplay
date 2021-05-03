#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include "ShaderLoader.h"
#include "Shader.h"
#include "EnvironmentMap.h"

struct DrawState {
    glm::mat4 projection, view, model;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    void ClearCurrentFrame();

    void Begin(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);
    void End();

    void DrawLines(GLuint VAO, float width, int points);

    void ApplyPhongShader(glm::vec3 lightPos, glm::vec3 phongParameter);

    void ApplyPRTShader();

    void SetEnvironment(const std::shared_ptr<EnvironmentMap>& envirMap);

    void DrawSkyBox();

private:
    static constexpr int MAXN = 64;
    DrawState stack[MAXN];
    int stacktop;

    GLuint _vaoBox, _vboBox;

    std::shared_ptr<Shader> _lineShader;
    std::shared_ptr<Shader> _phongShader;
    std::shared_ptr<Shader> _prtShader;
    std::shared_ptr<Shader> _skyBoxShader;


    std::shared_ptr<EnvironmentMap> _curEnvironmentMap;

    DrawState& getCurrentTransform();
    void initialize();
};