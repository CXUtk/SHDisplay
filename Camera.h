#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
class Camera {
public:
    Camera(float fovY, float aspect, float zNear, float zFar);
    ~Camera();

    void SetEyePos(glm::vec3 eye) { _eyePos = eye; }
    void SetLookAt(glm::vec3 pos) { _lookAt = pos; }

    glm::vec3 GetEyePos() const { return _eyePos; }
    glm::vec3 GetLookAt() const { return _lookAt; }

    glm::mat4 GetProjectionMatrix() const { return glm::perspective(_fovY, _aspectRatio, _zNear, _zFar); }
    glm::mat4 GetViewMatrix() const { return glm::lookAt(_eyePos, _lookAt, _up); }

private:
    glm::vec3 _eyePos, _lookAt, _up;
    float _fovY, _aspectRatio, _zNear, _zFar;
};