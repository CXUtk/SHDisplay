#include "EnvironmentMap.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stbi/stb_image.h>
#include <stbi/stb_image_write.h>

#include <Eigen/Dense>

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
        stbi_image_free(data);
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

glm::mat4 genMatrix(glm::mat3 prt) {

    /* Form the quadratic form matrix (see equations 11 and 12 in paper) */

    int col;
    float c1, c2, c3, c4, c5;
    c1 = 0.429043; c2 = 0.511664;
    c3 = 0.743125; c4 = 0.886227; c5 = 0.247708;

    std::vector<float> coeffs;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            coeffs.push_back(prt[i][j]);
        }
    }

    glm::mat4 result;

    result[0][0] = c1 * coeffs[8]; /* c1 L_{22}  */
    result[0][1] = c1 * coeffs[4]; /* c1 L_{2-2} */
    result[0][2] = c1 * coeffs[7]; /* c1 L_{21}  */
    result[0][3] = c2 * coeffs[3]; /* c2 L_{11}  */

    result[1][0] = c1 * coeffs[4]; /* c1 L_{2-2} */
    result[1][1] = -c1 * coeffs[8]; /*-c1 L_{22}  */
    result[1][2] = c1 * coeffs[5]; /* c1 L_{2-1} */
    result[1][3] = c2 * coeffs[1]; /* c2 L_{1-1} */

    result[2][0] = c1 * coeffs[7]; /* c1 L_{21}  */
    result[2][1] = c1 * coeffs[5]; /* c1 L_{2-1} */
    result[2][2] = c3 * coeffs[6]; /* c3 L_{20}  */
    result[2][3] = c2 * coeffs[2]; /* c2 L_{10}  */

    result[3][0] = c2 * coeffs[3]; /* c2 L_{11}  */
    result[3][1] = c2 * coeffs[1]; /* c2 L_{1-1} */
    result[3][2] = c2 * coeffs[2]; /* c2 L_{10}  */
    result[3][3] = c4 * coeffs[0] - c5 * coeffs[6];
    /* c4 L_{00} - c5 L_{20} */

    return result;
}

Eigen::VectorXf RotateSH(Eigen::VectorXf sh, Eigen::Matrix4f rotation) {

}

std::vector<glm::mat4> EnvironmentMap::GetUnshadowQuadraticForm() const {
    std::vector<glm::mat4> M;
    for (int i = 0; i < 3; i++) {
        M.push_back(genMatrix(_prt[i]));
    }
    return M;
}
