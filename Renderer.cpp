#include "Renderer.h"

Renderer::Renderer() {
    stacktop = 0;
    memset(stack, 0, sizeof(stack));

    initialize();
}

Renderer::~Renderer() {
}

void Renderer::ClearCurrentFrame() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Renderer::Begin(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) {
    stack[++stacktop] = DrawState{ projection, view, model };
}

void Renderer::End() {
    --stacktop;
}

void Renderer::DrawLines(GLuint VAO, float width, int points) {
    _lineShader->Apply();
    glLineWidth(width);

    _lineShader->SetParameter<glm::mat4>("transform", getCurrentTransform().projection);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, points);
    glBindVertexArray(0);
}

void Renderer::ApplyPhongShader(glm::vec3 lightPos, glm::vec3 phongParameter) {
    _phongShader->Apply();
    _phongShader->SetParameter<glm::mat4>("projection", getCurrentTransform().projection);
    _phongShader->SetParameter<glm::mat4>("view", getCurrentTransform().view);
    _phongShader->SetParameter<glm::mat4>("model", getCurrentTransform().model);

    _phongShader->SetParameter<glm::vec3>("uLightPos", glm::vec3(getCurrentTransform().view * glm::vec4(lightPos, 1.0f)));
    _phongShader->SetParameter<glm::vec3>("uLightParameter", phongParameter);
}

void Renderer::ApplyPRTShader() {
    _prtShader->Apply();
    _prtShader->SetParameter<glm::mat4>("projection", getCurrentTransform().projection);
    _prtShader->SetParameter<glm::mat4>("view", getCurrentTransform().view);
    _prtShader->SetParameter<glm::mat4>("model", getCurrentTransform().model);

    _prtShader->SetParameter<glm::mat3>("uEnvironmentSH.SH_R", SHMatrix[0]);
    _prtShader->SetParameter<glm::mat3>("uEnvironmentSH.SH_G", SHMatrix[1]);
    _prtShader->SetParameter<glm::mat3>("uEnvironmentSH.SH_B", SHMatrix[2]);
}



DrawState& Renderer::getCurrentTransform() {
    return stack[stacktop];
}

void Renderer::initialize() {
    _phongShader = std::make_shared<Shader>( loadVertexFragmantShader("../../../resources/shaders/phong.vs",
        "../../../resources/shaders/phong.frag"));

    _prtShader = std::make_shared<Shader>(loadVertexFragmantShader("../../../resources/shaders/PRT.vs",
        "../../../resources/shaders/PRT.frag"));


    FILE* SHFile = fopen("../../../resources/prt/CornellBox/PRT.txt", "r");
    int n;
    fscanf(SHFile, "%d", &n);

    for (int i = 0; i < 9; i++) {
        int s = i / 3;
        int t = i % 3;
        fscanf(SHFile, "%f %f %f", &SHMatrix[0][s][t], &SHMatrix[1][s][t], &SHMatrix[2][s][t]);
    }
    fclose(SHFile);
}

