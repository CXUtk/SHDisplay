#version 330 core
out vec4 fColor;

in vec3 fPos;
in vec3 fNormal;
in vec2 fTexCoord;

uniform vec3 uLightPos;
uniform vec3 uLightParameter;

void main(){
    vec3 V = normalize(-fPos);
    vec3 L = normalize(uLightPos - fPos);
    vec3 H = normalize(L + V);
    vec3 N = normalize(fNormal);

    vec3 ambient = vec3(uLightParameter.x);

    vec3 diffuse = max(0.0, dot(N, L)) * vec3(uLightParameter.y);
    
    vec3 specular = pow(max(0.0, dot(N, H)), 128.0) * vec3(uLightParameter.z);

    fColor = vec4(ambient + diffuse + specular, 1.0);
}