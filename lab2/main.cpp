#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "transform.h"

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

    sf::Clock clock {};
    float dt = 0;

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

        Transform {}.rotateZ(dt * 0.0000005f).apply_to(&camera.pos.x, 1);

        world_to_view_space(camera).apply_with(viewSpace, figureVer, vertexCount);

        switch (projection) 
        {
            case Projection::Perspective: 
                {
                    if (show_once) std::cout << "Perspective" << std::endl;
                    perspective_projection(pictureSpace, viewSpace, camera, vertexCount);
                    show_once = false;
                } 
                break;

            case Projection::Parallel: 
                {
                    if (show_once) std::cout << "Parallel" << std::endl;
                    parallel_projection(pictureSpace, viewSpace, vertexCount);
                    show_once = false;
                } 
                break;
        }

        picture_to_screen_space(screenSpace, pictureSpace, vertexCount, screen, camera.zoom);
        expand_to_final_vertex_array(finalVA, screenSpace, figureReb, segmentCount);
       
        finalVA[0].color = sf::Color::Green;
        finalVA[1].color = sf::Color::Green;

        window.clear();
        window.draw(finalVA, segmentCount * 2, sf::Lines);
        window.display();

        dt = (float) clock.restart().asMicroseconds();

    }

    return 0;
}