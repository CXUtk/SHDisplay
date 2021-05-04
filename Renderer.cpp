#include "Renderer.h"
#include "Canvas.h"

static float skyboxVertices[] = {        
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};


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
    Canvas& canvas = Canvas::GetInstance();

    _prtShader->Apply();
    _prtShader->SetParameter<glm::mat4>("projection", getCurrentTransform().projection);
    _prtShader->SetParameter<glm::mat4>("view", getCurrentTransform().view);
    _prtShader->SetParameter<glm::mat4>("model", getCurrentTransform().model);
    _prtShader->SetParameter<int>("uGammaCorrection", (int)canvas.GetGammaCorrection());
    _prtShader->SetParameter<int>("uShadowed", (int)(!canvas.IsUnshadowed()));


    if (canvas.IsUnshadowed()) {
        auto M = _curEnvironmentMap->GetUnshadowQuadraticForm();
        _prtShader->SetParameter<glm::mat4>("uEnvironmentSH.QUAD_R", M[0]);
        _prtShader->SetParameter<glm::mat4>("uEnvironmentSH.QUAD_G", M[1]);
        _prtShader->SetParameter<glm::mat4>("uEnvironmentSH.QUAD_B", M[2]);
    }
    else {
        _prtShader->SetParameter<glm::mat3>("uEnvironmentSH.SH_R", _curEnvironmentMap->GetLightFunction(0));
        _prtShader->SetParameter<glm::mat3>("uEnvironmentSH.SH_G", _curEnvironmentMap->GetLightFunction(1));
        _prtShader->SetParameter<glm::mat3>("uEnvironmentSH.SH_B", _curEnvironmentMap->GetLightFunction(2));
    }
}


void Renderer::SetEnvironment(const std::shared_ptr<EnvironmentMap>& envirMap) {
    _curEnvironmentMap = envirMap;
}

void Renderer::DrawSkyBox() {
    glDepthMask(GL_FALSE);
    _skyBoxShader->Apply();

    glBindVertexArray(_vaoBox);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _curEnvironmentMap->GetTexture());
    _skyBoxShader->SetParameter<glm::mat4>("projection", getCurrentTransform().projection);
    _skyBoxShader->SetParameter<glm::mat4>("view", getCurrentTransform().view);
    _skyBoxShader->SetParameter<int>("uCubeMap", 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
}



DrawState& Renderer::getCurrentTransform() {
    return stack[stacktop];
}

void Renderer::initialize() {
    _phongShader = std::make_shared<Shader>(loadVertexFragmantShader("../../../resources/shaders/phong.vs",
        "../../../resources/shaders/phong.frag"));

    _prtShader = std::make_shared<Shader>(loadVertexFragmantShader("../../../resources/shaders/PRT.vs",
        "../../../resources/shaders/PRT.frag"));

    _skyBoxShader = std::make_shared<Shader>(loadVertexFragmantShader("../../../resources/shaders/skybox.vs",
        "../../../resources/shaders/skybox.frag"));

    // Skybox VAO
    glGenVertexArrays(1, &_vaoBox);
    glGenBuffers(1, &_vboBox);

    glBindVertexArray(_vaoBox);
    glBindBuffer(GL_ARRAY_BUFFER, _vboBox);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

}

