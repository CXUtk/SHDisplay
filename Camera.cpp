#include "Camera.h"

Camera::Camera(float fovY, float aspect, float zNear, float zFar) : _fovY(fovY), _aspectRatio(aspect), _zNear(zNear), _zFar(zFar) {
    _eyePos = glm::vec3(0, 0, 5);
    _lookAt = glm::vec3(0, 0, 0);
    _up = glm::vec3(0, 1, 0);
}

Camera::~Camera() {
}
