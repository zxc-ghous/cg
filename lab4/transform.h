#pragma once
#include <math.h>
#include <vector>
#include <SFML/Graphics.hpp>

constexpr float TAU = 6.28318530718f;
constexpr float PI = TAU / 2.f;
constexpr float EPSILON = 0.0000001f;

struct vec3 
{
    struct 
    {
        float x;
        float y;
        float z;
    };
    float a[3];


    vec3 operator - () 
    {
        return vec3{ -x, -y, -z };
    }

    vec3 operator + (vec3 v) 
    {
        return { x + v.x, y + v.y, z + v.z };
    }

    vec3 operator - (vec3 v) 
    {
        return { x - v.x, y - v.y, z - v.z };
    }

    void operator += (vec3 v) 
    {
        (*this) = (*this) + v;
    }

    void operator -= (vec3 v) 
    {
        (*this) = (*this) - v;
    }

    static inline float euclidianDistance(vec3 a, vec3 b) 
    {
        return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
    }

    float norm() 
    {
        return vec3::euclidianDistance((*this), { 0, 0, 0 });
    }

    friend vec3 operator * (float a, vec3 v);

    vec3& normalize() 
    {
        (*this) = (1.f / (*this).norm()) * (*this);
        return (*this);
    }

    vec3 normalized() 
    {
        return (1.f / (*this).norm()) * (*this);
    }
};

inline vec3 operator * (float a, vec3 v) 
{
    return { a * v.x, a * v.y, a * v.z };
}

struct alignas(4) ivec2 {
    union {
        struct {
            wint_t x;
            wint_t y;
        };

        float a[2];
    };
};

struct alignas(4) vec2 {
    union {
        struct {
            float x;
            float y;
        };

        float a[2];
    };
};


struct Transform {
    float mat[16];
    Transform();
    Transform(Transform const&) = default;
    Transform& translate(vec3);
    Transform& scale(vec3);
    Transform& rotateX(float, float);
    Transform& rotateY(float, float);
    Transform& rotateZ(float, float);
    Transform& rotateX(float a) { return (*this).rotateX(cosf(a), sinf(a)); };
    Transform& rotateY(float a) { return (*this).rotateY(cosf(a), sinf(a)); };
    Transform& rotateZ(float a) { return (*this).rotateZ(cosf(a), sinf(a)); };
    Transform& operator *= (Transform const&);
    Transform operator * (Transform const& other) {
        return Transform{ (*this) } *= other;
    }
    void transformTo(float* va, size_t n);
    void transformWith(float* to, float* with, size_t n);
};

struct Camera 
{
    vec3 pos;
    float zoom = 25.f;
    void setPos(float x_, float y_, float z_)
    {
        pos.x = x_;
        pos.y = y_;
        pos.z = z_;
    }
};

struct Screen 
{
    unsigned width;
    unsigned height;
};

Transform worldToView(Camera);
void perspectiveProj(float* pp, float* va, Camera, size_t);
void parallelProj(float* pp, float* va, size_t);
void pictureToScreen(float* screenSpace, float* pictureSpace, size_t vertexCount, Screen screen, float);


