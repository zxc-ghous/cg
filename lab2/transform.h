#pragma once
#include <math.h>
#include <vector>
#include <SFML/Graphics.hpp>

constexpr float TAU = 6.28318530718f;
constexpr float EPSILON = 0.0000001f;

struct vec 
{
    struct 
    {
        float x;
        float y;
        float z;
    };
    float a[3];


    vec operator - () 
    {
        return vec{ -x, -y, -z };
    }

    vec operator + (vec v) 
    {
        return { x + v.x, y + v.y, z + v.z };
    }

    void operator += (vec v) 
    {
        (*this) = (*this) + v;
    }

    static inline float euclidianDistance(vec a, vec b) 
    {
        return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
    }

    float norm() 
    {
        return vec::euclidianDistance((*this), { 0, 0, 0 });
    }

    friend vec operator * (float a, vec v);

    vec& normalize() 
    {
        (*this) = (1.f / (*this).norm()) * (*this);
        return (*this);
    }

    vec normalized() 
    {
        return (1.f / (*this).norm()) * (*this);
    }
};

inline vec operator * (float a, vec v) 
{
    return { a * v.x, a * v.y, a * v.z };
}

struct Transform {
    float mat[16];
    Transform();
    Transform(Transform const&) = default;
    Transform& translate(vec);
    Transform& scale(vec);
    Transform& rotateX(float, float);
    Transform& rotateY(float, float);
    Transform& rotateZ(float, float);
    Transform& rotateX(float a) { return (*this).rotateX(cosf(a), sinf(a)); };
    Transform& rotateY(float a) { return (*this).rotateY(cosf(a), sinf(a)); };
    Transform& rotateZ(float a) { return (*this).rotateZ(cosf(a), sinf(a)); };
    Transform& operator *= (Transform const&);
    void apply_to(float* va, size_t n);
    void apply_with(float* to, float* with, size_t n);
};

struct Camera 
{
    vec pos;
    float zoom = 35.f;
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

Transform world_to_view_space(Camera);
void perspective_projection(float* pp, float* va, Camera, size_t);
void parallel_projection(float* pp, float* va, size_t);
void picture_to_screen_space(float* screenSpace, float* pictureSpace, size_t vertexCount, Screen screen, float); //картину на экран
void expand_to_final_vertex_array(sf::Vertex* finalVA, float* screenSpace, int* ia, size_t segmentCount);

