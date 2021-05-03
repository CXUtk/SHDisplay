#include "EnvironmentMap.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stbi/stb_image.h>
#include <stbi/stb_image_write.h>

std::string names[6] = {
    "posx.jpg",
    "negx.jpg",
    "posy.jpg",
    "negy.jpg",
    "posz.jpg",
    "negz.jpg",
};

EnvironmentMap::EnvironmentMap(const std::string& path) {
    glGenTextures(1, &_cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemapTexture);

    int width, height, nrChannels;
    unsigned char* data;
    for (int i = 0; i < 6; i++) {
        data = stbi_load((path + names[i]).c_str(), &width, &height, &nrChannels, 0);
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
        );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    FILE* SHFile = fopen((path + "PRT.txt").c_str(), "r");

    if (!SHFile) {
        fprintf(stderr, "Cannot open PRT file\n");
        return;
    }
    int n;
    fscanf(SHFile, "%d", &n);

    for (int i = 0; i < 9; i++) {
        int s = i / 3;
        int t = i % 3;
        fscanf(SHFile, "%f %f %f", &_prt[0][s][t], &_prt[1][s][t], &_prt[2][s][t]);
    }
    fclose(SHFile);

}

EnvironmentMap::~EnvironmentMap() {
}