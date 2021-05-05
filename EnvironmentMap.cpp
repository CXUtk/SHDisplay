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


void SHEval3(const float fX, const float fY, const float fZ, float* pSH) {
    float c;
    c = 0.282095;
    pSH[0] = c;

    c = 0.488603;
    pSH[1] = (c * fY);
    pSH[2] = (c * fZ);
    pSH[3] = (c * fX);

    c = 1.092548;
    pSH[4] = (c * fX * fY);
    pSH[5] = (c * fY * fZ);
    pSH[7] = (c * fZ * fX);

    c = 0.315392;
    pSH[6] = c * (3 * fZ * fZ - 1);

    c = 0.546274;
    pSH[8] = c * (fX * fX - fY * fY);
}


Eigen::Matrix3f EvalRotateLevel1(const Eigen::Matrix4f& rotation) {
    Eigen::Vector4f base[3] = {
        Eigen::Vector4f(0, 1, 0, 0),
        Eigen::Vector4f(0, 0, 1, 0),
        Eigen::Vector4f(1, 0, 0, 0),
    };
    Eigen::Matrix3f A, M;
    for (int i = 0; i < 3; i++) {
        float shv[9];
        SHEval3(base[i].x(), base[i].y(), base[i].z(), shv);
        A.col(i) = Eigen::Vector3f(shv[1], shv[2], shv[3]);
    }
    A = A.inverse().eval();
    for (int i = 0; i < 3; i++) {
        base[i] = rotation * base[i];
        float shv[9];
        SHEval3(base[i].x(), base[i].y(), base[i].z(), shv);
        M.col(i) = Eigen::Vector3f(shv[1], shv[2], shv[3]);
    }
    return M * A;
}

Eigen::MatrixXf EvalRotateLevel2(const Eigen::Matrix4f& rotation) {
    float k = std::sqrt(2) / 2;
    Eigen::Vector4f base[5] = {
        Eigen::Vector4f(1, 0, 0, 0),
        Eigen::Vector4f(0, 0, 1, 0),
        Eigen::Vector4f(k, k, 0, 0),
        Eigen::Vector4f(k, 0, k, 0),
        Eigen::Vector4f(0, k, k, 0),
    };
    Eigen::MatrixXf A(5, 5), M(5, 5);
    for (int i = 0; i < 5; i++) {
        float shv[9];
        SHEval3(base[i].x(), base[i].y(), base[i].z(), shv);
        Eigen::VectorXf v(5);
        v << shv[4], shv[5], shv[6], shv[7], shv[8];
        A.col(i) = v;
    }
    A = A.inverse().eval();
    for (int i = 0; i < 5; i++) {
        base[i] = rotation * base[i];
        float shv[9];
        SHEval3(base[i].x(), base[i].y(), base[i].z(), shv);
        Eigen::VectorXf v(5);
        v << shv[4], shv[5], shv[6], shv[7], shv[8];
        M.col(i) = v;
    }
    return M * A;
}

Eigen::VectorXf RotateSH(const Eigen::VectorXf& sh, const Eigen::Matrix3f& R1, const Eigen::MatrixXf& R2) {
    Eigen::VectorXf output(9);
    output(0) = sh(0);

    Eigen::Vector3f L1(sh(1), sh(2), sh(3));
    L1 = R1 * L1;
    output(1) = L1(0);
    output(2) = L1(1);
    output(3) = L1(2);

    Eigen::VectorXf L2(5);
    L2 << sh(4), sh(5), sh(6), sh(7), sh(8);
    L2 = R2 * L2;
    output(4) = L2(0);
    output(5) = L2(1);
    output(6) = L2(2);
    output(7) = L2(3);
    output(8) = L2(4);

    return output;
}


std::vector<glm::mat3> EnvironmentMap::GetLightFunction(const glm::mat4& rotation) const {
    std::vector<glm::mat3> res;
    Eigen::Matrix4f R;
    for (int i = 0; i < 4; i++) {
        Eigen::Vector4f V;
        for (int j = 0; j < 4; j++) {
            V(j) = rotation[i][j];
        }
        R.col(i) = V;
    }

    auto R1 = EvalRotateLevel1(R);
    auto R2 = EvalRotateLevel2(R);

    for (int id = 0; id < 3; id++) {
        Eigen::VectorXf SH(9);
        for (int i = 0; i < 9; i++) {
            int c = i / 3;
            int r = i % 3;
            SH(i) = _prt[id][c][r];
        }
        SH = RotateSH(SH, R1, R2);
        glm::mat3 rotatedPRT{};
        for (int i = 0; i < 9; i++) {
            int c = i / 3;
            int r = i % 3;
            rotatedPRT[c][r] = SH(i);
        }
        res.push_back(rotatedPRT);
    }
    return res;
}

std::vector<glm::mat4> EnvironmentMap::GetUnshadowQuadraticForm(const glm::mat4& rotation) const {
    auto prt = GetLightFunction(rotation);
    std::vector<glm::mat4> M;
    for (int i = 0; i < 3; i++) {
        M.push_back(genMatrix(prt[i]));
    }
    return M;
}
