#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in mat3 vPRT;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform int uGammaCorrection;

struct EnvironmentSH{
    mat3 SH_R;
    mat3 SH_G;
    mat3 SH_B;
};

uniform EnvironmentSH uEnvironmentSH;

out vec4 fColor;

float multiply(mat3 A, mat3 B){
    return dot(A[0], B[0]) + dot(A[1], B[1]) + dot(A[2], B[2]);
}

void main(){
    gl_Position = projection * view * model * vec4(vPos, 1);

    vec3 color = vec3(multiply(uEnvironmentSH.SH_R, vPRT),
        multiply(uEnvironmentSH.SH_G, vPRT),
        multiply(uEnvironmentSH.SH_B, vPRT));

    color /= 3.1415926;

    if (uGammaCorrection != 0) {
        color.r = pow(color.r, 1 / 2.2);
        color.g = pow(color.g, 1 / 2.2);
        color.b = pow(color.b, 1 / 2.2);
    }

    fColor = vec4(
        color,
        1.0
    );
}

