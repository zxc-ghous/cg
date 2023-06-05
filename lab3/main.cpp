#include "transform.h"
#include <iostream>
#include <SFML/Graphics.hpp>


struct House 
{
        static constexpr int vertexCount = 10;
        static constexpr int triCount = 14;

        float va[vertexCount * 3]   
        {
            3.f, -2.f, 0.f,     //0
            3.f, 2.f, 0.f,      //1
            3.f, 2.f, 2.f,      //2
            3.f, -2.f, 2.f,     //3
            3.f, 0.f, 3.f,      //4
            -3.f, -2.f, 0.f,    //5
            -3.f, 2.f, 0.f,     //6
            -3.f, 2.f, 2.f,     //7
            -3.f, -2.f, 2.f,    //8
            -3.f, 0.f, 3.f,     //9
        };

        int tri[triCount * 3]
        {
            0, 1, 2,
            3, 0, 2,
            2, 4, 3,
            1, 6, 7,
            2, 1, 7,
            2, 9, 4,
            2, 7, 9,
            7, 9, 8,
            5, 6, 7,
            7, 8, 5,
            3 ,4, 8,
            4 ,9, 8,
            0, 5, 8,
            0, 8, 3
        };

        sf::Color triColors[triCount]{};

        float centerPoint[3]{};
        float* W;

        House()
        {
            enum Var : int { x, y, z };

            W = (float*)malloc(triCount * 4 * sizeof(float)); 
            {
                if (!W) 
                {
                    exit(EXIT_FAILURE);
                }
            }

            for (int i = 0; i < vertexCount; i += 1) 
            {
                centerPoint[x] += va[3 * i + x] * (1.f / vertexCount);
                centerPoint[y] += va[3 * i + y] * (1.f / vertexCount);
                centerPoint[z] += va[3 * i + z] * (1.f / vertexCount);
            }

            for (int i = 0; i < triCount; i += 1) 
            {
                auto const static p = [this, &i](Var v, int j) -> float {
                    return va[3 * tri[3 * i + (j - 1)] + v];
                };

                float a
                {
                    (p(y, 3) - p(y, 1)) * (p(z, 2) - p(z, 1)) -
                    (p(y, 2) - p(y, 1)) * (p(z, 3) - p(z, 1))
                };

                float b
                {
                    (p(x, 2) - p(x, 1)) * (p(z, 3) - p(z, 1)) -
                    (p(x, 3) - p(x, 1)) * (p(z, 2) - p(z, 1))
                };

                float c
                {
                    (p(x, 3) - p(x, 1)) * (p(y, 2) - p(y, 1)) -
                    (p(x, 2) - p(x, 1)) * (p(y, 3) - p(y, 1))
                };

                float d
                {
                    -a * p(x, 1)
                    - b * p(y, 1)
                    - c * p(z, 1)
                };

                if (a * centerPoint[x] + b * centerPoint[y] + c * centerPoint[z] + d < 0.f) 
                {
                    a *= -1.f;
                    b *= -1.f;
                    c *= -1.f;
                    d *= -1.f;
                }

                W[4 * i + 0] = a;
                W[4 * i + 1] = b;
                W[4 * i + 2] = c;
                W[4 * i + 3] = d;
            }
        }

        ~House() 
        {
            free(W);
        }

} house{};



Screen screen{ 800, 600 };
Camera camera{ {6, 6, 6} }; 
Camera visCamera{ {6, 6, 6} }; 
struct CameraController
{
        Camera& cam = ::camera;
        float dragSpeed = 0.0000008f;
        float pitchThreshold = 2.f;
        float zoomSpeed = 0.8f;
        struct { int x, y; } prevCurPos{ 0, 0 };

        void dragCamera(float dx, float dy)
        {

            Transform{}.rotateZ(dx * dragSpeed).transformTo(&cam.pos.x, 1);

            float d = hypotf(cam.pos.x, cam.pos.y);

            if ((d > pitchThreshold) || (cam.pos.z > 0 && dy < 0) || (cam.pos.z < 0 && dy > 0)) {
                float cosu = cam.pos.y / d;
                float sinu = cam.pos.x / d;
                Transform{}
                    .rotateZ(cosu, -sinu)
                    .rotateX(-dy * dragSpeed)
                    .rotateZ(cosu, sinu)
                    .transformTo(&cam.pos.x, 1);
            }
        }
} camController;



float* pictureSpace = (float*)malloc(house.vertexCount * 2 * sizeof(float));
float* screenSpace = (float*)malloc(house.vertexCount * 2 * sizeof(float));
float* viewSpace = (float*)malloc(house.vertexCount * 3 * sizeof(float));
sf::Vertex* finalVA = (sf::Vertex*)malloc(house.triCount * 3 * sizeof(sf::Vertex));



enum class Projection : int 
{
    Perspective,
    Parallel,
} projection;

void flattenIVA(sf::Vertex* finalVA, float const* screenSpace, int const* tri, int triCount)
{
    static int* orderedTri = (int*)malloc(sizeof(int) * triCount);
    auto static const isCCW{ [=](int i) -> bool 
    {
            float x[3] 
            {
                screenSpace[2 * tri[3 * i + 0] + 0],
                screenSpace[2 * tri[3 * i + 1] + 0],
                screenSpace[2 * tri[3 * i + 2] + 0],
            };

            float y[3] 
            {
                screenSpace[2 * tri[3 * i + 0] + 1],
                screenSpace[2 * tri[3 * i + 1] + 1],
                screenSpace[2 * tri[3 * i + 2] + 1],
            };

            return x[0] * y[1] + x[1] * y[2] + x[2] * y[0] < y[0] * x[1] + y[1] * x[2] + y[2] * x[0];
    } };

    {
            int i = 0;
            int j = triCount - 1;
            for (int k = 0; k < triCount; k += 1) 
            {
                if (isCCW(k)) 
                {
                    orderedTri[j--] = k;
                }
                else 
                {
                    orderedTri[i++] = k;
                }
            }
    }

        for (int i = 0; i < triCount; i += 1) 
        {
            for (int j = 0; j < 3; j += 1) 
            {
                finalVA[3 * i + j] = sf::Vertex
                {
                    sf::Vector2f {
                        screenSpace[2 * tri[3 * orderedTri[i] + j] + 0],
                        screenSpace[2 * tri[3 * orderedTri[i] + j] + 1]
                    },
                    house.triColors[orderedTri[i]]
                };
            }
        }
}


void markVisibleTris(Projection projection, House& house, Camera& cam) 
{
    auto static const isVisible = [&](int i) -> bool 
    {
        bool r = false;
        switch (projection) 
        {
        case Projection::Perspective: 
        {
            r = (
                house.W[4 * i + 0] * cam.pos.x +
                house.W[4 * i + 1] * cam.pos.y +
                house.W[4 * i + 2] * cam.pos.z +
                house.W[4 * i + 3]
                ) < 0.f;
        } break;

        case Projection::Parallel: 
        {
            r = (
                house.W[4 * i + 0] * cam.pos.x +
                house.W[4 * i + 1] * cam.pos.y +
                house.W[4 * i + 2] * cam.pos.z
                ) < 0.f;
        } break;
        }

        return r;
    };

    for (int i = 0; i < house.triCount; i += 1) 
    {
        if (isVisible(i)) 
        {
            house.triColors[i] = sf::Color::White;
        }
        else 
        {
            house.triColors[i] = sf::Color::Red;
        }
    }
    house.triColors[0] = sf::Color::Blue;
    house.triColors[1] = sf::Color::Blue;
    house.triColors[2] = sf::Color::Magenta;
    house.triColors[3] = sf::Color::Green;
    house.triColors[4] = sf::Color::Green;
    house.triColors[5] = sf::Color::Yellow;
    house.triColors[6] = sf::Color::Yellow;
}


int main()
{
    auto window = sf::RenderWindow{sf::VideoMode {screen.width, screen.height}, "lab3", sf::Style::Default};
    auto event = sf::Event{};
    bool show_once = false;


    while (window.isOpen())
    {

        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
            {
                window.close();
            }
            break;

            case sf::Event::KeyPressed:
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::P:
                    {
                        projection = Projection((int)projection ^ 1);
                        show_once = true;
                    }
                    break;

                    case sf::Keyboard::Q:
                    {
                        camController.cam.zoom -= camController.zoomSpeed;
                    }
                    break;

                    case sf::Keyboard::E:
                    {
                        camController.cam.zoom += camController.zoomSpeed;
                    }
                    break;

                    case sf::Keyboard::S:
                    {
                        float x, y, z;
                        std::cout << "set new pos" << std::endl;
                        std::cin >> x >> y >> z;
                        camera.setPos(x, y, z);
                    }
                    break;

                    case sf::Keyboard::Z:
                    {
                        camController.dragCamera(10000, 0.f);
                    }
                    break;

                    case sf::Keyboard::X:
                    {
                        camController.dragCamera(-10000, 0.f);
                    }
                    break;
                }
            }
            break;
            }
        }


        markVisibleTris(projection, house, visCamera);
        worldToView(camera).transformWith(viewSpace, house.va, house.vertexCount);
        switch (projection)
        {
        case Projection::Perspective:
        {
            if (show_once) std::cout << "Perspective" << std::endl;
            perspectiveProj(pictureSpace, viewSpace, camera, house.vertexCount);
            show_once = false;
        }
        break;

        case Projection::Parallel:
        {
            if (show_once) std::cout << "Parallel" << std::endl;
            parallelProj(pictureSpace, viewSpace, house.vertexCount);
            show_once = false;
        }
        break;
        }
        pictureToScreen(screenSpace, pictureSpace, house.vertexCount, screen, camera.zoom);
        flattenIVA(finalVA, screenSpace, house.tri, house.triCount);



        window.clear();
        window.draw(finalVA, house.triCount * 3, sf::Triangles);
        window.display();
    }
}