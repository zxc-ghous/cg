#include "transform.h"
#include "common.h"
#include <string.h>

// any of the transform methods: 0.7

Transform::Transform() : mat{
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
} {}

Transform& Transform::translate(Vec3 v) {
    auto shift = Transform{};
    shift.mat[12] = v.x;
    shift.mat[13] = v.y;
    shift.mat[14] = v.z;

    return (*this) *= shift;
}

Transform& Transform::scale(Vec3 v) {
    auto scale = Transform{};
    scale.mat[0] = v.x;
    scale.mat[5] = v.y;
    scale.mat[10] = v.z;

    return (*this) *= scale;
}

Transform& Transform::rotateX(float cosa, float sina) {
    auto rotateX = Transform{};
    rotateX.mat[5] = cosa;
    rotateX.mat[10] = cosa;
    rotateX.mat[6] = -sina;
    rotateX.mat[9] = sina;

    return (*this) *= rotateX;
}

Transform& Transform::rotateY(float cosa, float sina) {
    auto rotateX = Transform{};
    rotateX.mat[0] = cosa;
    rotateX.mat[10] = cosa;
    rotateX.mat[8] = -sina;
    rotateX.mat[2] = sina;

    return (*this) *= rotateX;
}

Transform& Transform::rotateZ(float cosa, float sina) {
    auto rotateX = Transform{};
    rotateX.mat[0] = cosa;
    rotateX.mat[5] = cosa;
    rotateX.mat[1] = -sina;
    rotateX.mat[4] = sina;

    return (*this) *= rotateX;
}

Transform& Transform::operator *= (Transform const& b) {
    Transform a{ (*this) };

    for (int i = 0; i < 4; i += 1) {
        for (int j = 0; j < 4; j += 1) {
            float s = 0.f;
            for (int k = 0; k < 4; k += 1) {
                s += a.mat[4 * i + k] * b.mat[4 * k + j];
            } mat[4 * i + j] = s;
        }
    }

    return *this;
}

void Transform::applyTo(float* va, size_t n) { //va - vertex array, плоский массив по трем координатам, точка имеет 3 компоненты
    for (size_t i = 0; i < n * 3; i += 3) {
        Vec3 v{ va[i], va[i + 1], va[i + 2] };
        va[i] = v.x * mat[0] + v.y * mat[4] + v.z * mat[8] + 1.f * mat[12];
        va[i + 1] = v.x * mat[1] + v.y * mat[5] + v.z * mat[9] + 1.f * mat[13];
        va[i + 2] = v.x * mat[2] + v.y * mat[6] + v.z * mat[10] + 1.f * mat[14];
    }
}

void Transform::applyWith(float* __restrict to, float* __restrict with, size_t n) { //with буфер не меняется, в to записываем
    for (size_t i = 0; i < n * 3; i += 3) {
        Vec3 v{ with[i], with[i + 1], with[i + 2] };
        to[i] = v.x * mat[0] + v.y * mat[4] + v.z * mat[8] + 1.f * mat[12];
        to[i + 1] = v.x * mat[1] + v.y * mat[5] + v.z * mat[9] + 1.f * mat[13];
        to[i + 2] = v.x * mat[2] + v.y * mat[6] + v.z * mat[10] + 1.f * mat[14];
    }
}


Transform worldToView(Camera cam) {
    float d;

    /* 1..3 */
    let viewTransform = Transform{}
        .translate(-cam.pos)
        .scale({ -1.f, 1.f, 1.f })
        .rotateX(.25f * TAU)
        ;

    /* 4 */
    {
        d = hypotf(cam.pos.x, cam.pos.y);
        float cosu;
        float sinu;

        if (d < EPSILON) {
            cosu = 1.f;
            sinu = 0.f;
        }
        else {
            cosu = cam.pos.y / d;
            sinu = cam.pos.x / d;
        }

        viewTransform *= Transform{}.rotateY(cosu, sinu);
    }

    /* 5 */
    {
        float s = hypotf(d, cam.pos.z);
        float cosw;
        float sinw;

        if (s < EPSILON) {
            cosw = 1.f;
            sinw = 0.f;
        }
        else {
            cosw = d / s;
            sinw = cam.pos.z / s;
        }

        viewTransform *= Transform{}.rotateX(cosw, sinw);
    }

    return viewTransform;
}

void perspectiveProj(float* __restrict pp, float* __restrict va, Camera camera, size_t n) {
    float s = Vec3::euclidianDistance(camera.pos, { 0.f, 0.f, 0.f });
    for (size_t i = 0; i < n; i += 1) {
        pp[2 * i + 0] = va[3 * i + 0] * (s / va[3 * i + 2]);
        pp[2 * i + 1] = va[3 * i + 1] * (s / va[3 * i + 2]);
    }
}

void parallelProj(float* __restrict pp, float* __restrict va, size_t n) {
    for (size_t i = 0; i < n; i += 1) {
        pp[2 * i + 0] = va[3 * i + 0];
        pp[2 * i + 1] = va[3 * i + 1];
    }
}

void pictureToScreen(float* __restrict screenSpace, float* __restrict pictureSpace, size_t vertexCount, Screen screen, float zoom) {
    for (size_t i = 0; i < vertexCount; i += 1) {
        screenSpace[2 * i + 0] = pictureSpace[2 * i + 0] * zoom + screen.width / 2.f;
        screenSpace[2 * i + 1] = -pictureSpace[2 * i + 1] * zoom + screen.height / 2.f;
    }
}

void flattenIVA(sf::Vertex* __restrict finalVA, float* __restrict screenSpace, int* __restrict ia, size_t segmentCount) { //в большой массив с вертексами
    for (size_t i = 0; i < segmentCount; i += 1) {
        finalVA[2 * i + 0] = sf::Vertex{
            sf::Vector2f {
                screenSpace[2 * ia[2 * i + 0] + 0],
                screenSpace[2 * ia[2 * i + 0] + 1]
            },
            sf::Color::White
        };

        finalVA[2 * i + 1] = sf::Vertex{
            sf::Vector2f {
                screenSpace[2 * ia[2 * i + 1] + 0],
                screenSpace[2 * ia[2 * i + 1] + 1]
            },
            sf::Color::White
        };
    }
}