#version 330 core

layout (location = 0) in vec3 vPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 fPos;

void main(){
    mat4 _view = view;
    _view[3].xyz = vec3(0, 0, 0);
    gl_Position = projection * _view * model * vec4(vPos, 1);
    fPos = vPos;
}

