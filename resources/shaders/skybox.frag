#version 330 core
out vec4 outColor;

in vec3 fPos;

uniform samplerCube uCubeMap;

void main(){
    outColor = texture(uCubeMap, fPos);
}