#include "transform.h"
#include <iostream>
#include <SFML/Graphics.hpp>

float figureVer[]{
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
int figureReb[]{
    3, 8,
    3, 0,
    3, 2,
    3, 4,
    0, 5,
    0, 1,
    1, 2,
    1, 6,
    4, 2,
    2, 7,
    4, 9,
    5, 6,
    6, 7,
    7, 8,
    7, 9,
    5, 8,
    9, 8,
};

size_t vertexCount = sizeof(figureVer) / (sizeof(float) * 3);
size_t segmentCount = sizeof(figureReb) / (sizeof(float) * 2);

float* pictureSpace = (float*)malloc(vertexCount * 2 * sizeof(float));
float* screenSpace = (float*)malloc(vertexCount * 2 * sizeof(float));
float* viewSpace = (float*)malloc(sizeof(figureVer));
sf::Vertex* finalVA = (sf::Vertex*)malloc((segmentCount * 2) * sizeof(sf::Vertex));

Screen screen{ 800, 600 };
Camera camera{ {6, 6, 6} };

struct CameraController 
{
    Camera& cam = ::camera;
    float dragSpeed = 0.0000008f;
    float pitchThreshold = 2.f;
    float zoomSpeed = 0.8f;
    struct { int x, y; } prevCurPos{ 0, 0 };

    void dragCamera(float dx, float dy) 
    {

        Transform{}.rotateZ(dx * dragSpeed).applyTo(&cam.pos.x, 1);

        float d = hypotf(cam.pos.x, cam.pos.y);

        if ((d > pitchThreshold) || (cam.pos.z > 0 && dy < 0) || (cam.pos.z < 0 && dy > 0)) {
            float cosu = cam.pos.y / d;
            float sinu = cam.pos.x / d;
            Transform{}
                .rotateZ(cosu, -sinu)
                .rotateX(-dy * dragSpeed)
                .rotateZ(cosu, sinu)
                .applyTo(&cam.pos.x, 1);
        }
    }
} camController;

int main() 
{
    auto window = sf::RenderWindow{ sf::VideoMode {screen.width, screen.height}, "lab2", sf::Style::Default };
    auto event = sf::Event{};
    bool show_once = false;

    enum class Projection
    {
        Perspective,
        Parallel,
    } 
    projection = Projection::Perspective;


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
                    }
                } 
                break;
            }
        }

        camController.dragCamera(100, 0.f);

        worldToView(camera).applyWith(viewSpace, figureVer, vertexCount);

        switch (projection) 
        {
            case Projection::Perspective: 
                {
                    if (show_once) std::cout << "Perspective" << std::endl;
                    perspectiveProj(pictureSpace, viewSpace, camera, vertexCount);
                    show_once = false;
                } 
                break;

            case Projection::Parallel: 
                {
                    if (show_once) std::cout << "Parallel" << std::endl;
                    parallelProj(pictureSpace, viewSpace, vertexCount);
                    show_once = false;
                } 
                break;
        }

        pictureToScreen(screenSpace, pictureSpace, vertexCount, screen, camera.zoom);
        flattenIVA(finalVA, screenSpace, figureReb, segmentCount);
       
        for (int i = 0; i < segmentCount; i ++)
        {
            finalVA[i].color = sf::Color::Red;
        }

        window.clear();
        window.draw(finalVA, segmentCount * 2, sf::Lines);
        window.display();

    }

    return 0;
}