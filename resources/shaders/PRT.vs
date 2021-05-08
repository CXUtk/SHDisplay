#version 450 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

layout (location = 3) in vec3 vPRT_R1;
layout (location = 4) in vec3 vPRT_R2;
layout (location = 5) in vec3 vPRT_R3;
layout (location = 6) in vec3 vPRT_G1;
layout (location = 7) in vec3 vPRT_G2;
layout (location = 8) in vec3 vPRT_G3;
layout (location = 9) in vec3 vPRT_B1;
layout (location = 10) in vec3 vPRT_B2;
layout (location = 11) in vec3 vPRT_B3;



uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform int uGammaCorrection;
uniform int uMode;
uniform vec3 uColor;

struct EnvironmentSH{
    mat3 SH_R;
    mat3 SH_G;
    mat3 SH_B;
    mat4 QUAD_R;
    mat4 QUAD_G;
    mat4 QUAD_B;
};

uniform EnvironmentSH uEnvironmentSH;

out vec4 fColor;

float multiply(mat3 A, mat3 B){
    return dot(A[0], B[0]) + dot(A[1], B[1]) + dot(A[2], B[2]);
}

void main(){
    gl_Position = projection * view * model * vec4(vPos, 1);

    vec3 color;
    
    if(uMode == 0){
        vec4 N = vec4(normalize(vNormal), 1.0);
        color = vec3(dot(N, uEnvironmentSH.QUAD_R * N), 
                    dot(N, uEnvironmentSH.QUAD_G * N),
                    dot(N, uEnvironmentSH.QUAD_B * N));
    }
    else{
        mat3 vPRT_R = mat3(vPRT_R1, vPRT_R2, vPRT_R3);
        mat3 vPRT_G = mat3(vPRT_G1, vPRT_G2, vPRT_G3);
        mat3 vPRT_B = mat3(vPRT_B1, vPRT_B2, vPRT_B3);
        color = vec3(multiply(uEnvironmentSH.SH_R, vPRT_R),
            multiply(uEnvironmentSH.SH_G, vPRT_G),
            multiply(uEnvironmentSH.SH_B, vPRT_B));
    }

    color *= uColor;
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

