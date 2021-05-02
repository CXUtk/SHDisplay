#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 fPos;
out vec3 fNormal;
out vec2 fTexCoord;

void main(){
    mat4 viewModel = view * model;
    vec4 pos = viewModel * vec4(vPos, 1);
    gl_Position = projection * pos;

    fPos = pos.xyz;
    fNormal = (transpose(inverse(viewModel)) * vec4(vNormal, 0)).xyz;
    fTexCoord = vTexCoord;
}

