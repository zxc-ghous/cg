#pragma once
#include "common.h"
#include <math.h>
#include <vector>
#include <SFML/Graphics.hpp>

constexpr float TAU = 6.28318530718f;
constexpr float EPSILON = 0.0000001f;

struct alignas(4) Vec3 {
    union {
        struct {
            float x;
            float y;
            float z;
        };

        float a[3];
    };

    Vec3 operator - () {
        return Vec3{ -x, -y, -z };
    }

    Vec3 operator + (Vec3 v) {
        return { x + v.x, y + v.y, z + v.z };
    }

    void operator += (Vec3 v) {
        self = self + v;
    }

    static inline float euclidianDistance(Vec3 a, Vec3 b) {
        return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
    }

    float norm() {
        return Vec3::euclidianDistance(self, { 0, 0, 0 });
    }

    friend Vec3 operator * (float a, Vec3 v);

    Vec3& normalize() {
        self = (1.f / self.norm()) * self;
        return self;
    }

    Vec3 normalized() {
        return (1.f / self.norm()) * self;
    }
};

inline Vec3 operator * (float a, Vec3 v) {
    return { a * v.x, a * v.y, a * v.z };
}

struct Transform {
    float mat[16];
    Transform();
    Transform(Transform const&) = default;
    Transform& translate(Vec3);
    Transform& scale(Vec3);
    Transform& rotateX(float, float);
    Transform& rotateY(float, float);
    Transform& rotateZ(float, float);
    Transform& rotateX(float a) { return self.rotateX(cosf(a), sinf(a)); };
    Transform& rotateY(float a) { return self.rotateY(cosf(a), sinf(a)); };
    Transform& rotateZ(float a) { return self.rotateZ(cosf(a), sinf(a)); };
    Transform& operator *= (Transform const&);
    void applyTo(float*, size_t);
    void applyWith(float* __restrict, float* __restrict, size_t);
};

struct Camera {
    Vec3 pos;
    float zoom = 25.f;
    //cam always points to world origin
    //Vec3 dir;
};

struct Screen {
    unsigned width;
    unsigned height;
};

Transform worldToView(Camera);
void perspectiveProj(float* __restrict, float* __restrict, Camera, size_t);
void parallelProj(float* __restrict, float* __restrict, size_t);
void pictureToScreen(float* __restrict, float* __restrict, size_t, Screen, float); //картину на экран
void flattenIVA(sf::Vertex* __restrict, float* __restrict, int* __restrict, size_t);