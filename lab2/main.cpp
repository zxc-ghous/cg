#include "common.h"
#include "transform.h"
#include <SFML/Graphics.hpp>

static float prismVA[]{
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
static int prismIA[]{
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

constexpr size_t vertexCount = sizeof(prismVA) / (sizeof(float) * 3);
constexpr size_t segmentCount = sizeof(prismIA) / (sizeof(float) * 2);

static float* pictureSpace = (float*)malloc(vertexCount * 2 * sizeof(float));
static float* screenSpace = (float*)malloc(vertexCount * 2 * sizeof(float));
static float* viewSpace = (float*)malloc(sizeof(prismVA));
static sf::Vertex* finalVA = (sf::Vertex*)malloc((segmentCount * 2) * sizeof(sf::Vertex));

constexpr Screen screen{ 800, 600 };
static Camera camera{ {6, 6, 6} };

static struct CameraController {
    Camera& cam = ::camera;
    bool isDragging = true;
    float dragSpeed = 0.0000008f;
    float slideSpeed = 1.25f;
    float zoomSpeed = 0.8f;
    float pitchThreshold = 2.f;
    struct { int x, y; } prevCurPos{ 0, 0 };

    void dragCamera(float dx, float dy) {
        if (!isDragging) return;

        Transform{}.rotateZ(dx * dragSpeed).applyTo(cam.pos.a, 1);

        float d = hypotf(cam.pos.x, cam.pos.y);

        if ((d > pitchThreshold) || (cam.pos.z > 0 && dy < 0) || (cam.pos.z < 0 && dy > 0)) {
            float cosu = cam.pos.y / d;
            float sinu = cam.pos.x / d;
            Transform{}
                .rotateZ(cosu, -sinu)
                .rotateX(-dy * dragSpeed)
                .rotateZ(cosu, sinu)
                .applyTo(cam.pos.a, 1);
        }
    }
} camController;

int main() {
    let window = sf::RenderWindow{
        sf::VideoMode {screen.width, screen.height},
        "lab2",
        sf::Style::Default,
        sf::ContextSettings {
            0, 0, 0, 1, 1,
            sf::ContextSettings::Default,
            false
        }
    };
    let event = sf::Event{};

    enum class Projection : int {
        Perspective,
        Parallel,
    } projection = Projection::Perspective;

    float dt = 0;
    sf::Clock static clock{};

    while (window.isOpen()) {

        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed: {
                window.close();
            } break;

            case sf::Event::KeyPressed: {
                switch (event.key.code) {
                case sf::Keyboard::Escape: {
                    window.close();
                } break;

                case sf::Keyboard::P: {
                    projection = Projection((int)projection ^ 1);
                } break;

                case sf::Keyboard::Q: {
                    camController.cam.zoom -= camController.zoomSpeed;
                } break;

                case sf::Keyboard::E: {
                    camController.cam.zoom += camController.zoomSpeed;
                } break;

                case sf::Keyboard::LShift: {
                    camController.cam.pos += camController.slideSpeed * camController.cam.pos.normalized();
                } break;

                case sf::Keyboard::LControl: {
                    camController.cam.pos += -camController.slideSpeed * camController.cam.pos.normalized();
                } break;
                }
            } break;
            }
        }

        camController.dragCamera(100, 0.f);

        worldToView(camera).applyWith(viewSpace, prismVA, vertexCount);

        switch (projection) {
        case Projection::Perspective: {
            perspectiveProj(pictureSpace, viewSpace, camera, vertexCount);
        } break;

        case Projection::Parallel: {
            parallelProj(pictureSpace, viewSpace, vertexCount);
        } break;
        }

        pictureToScreen(screenSpace, pictureSpace, vertexCount, screen, camera.zoom);
        flattenIVA(finalVA, screenSpace, prismIA, segmentCount);

        {
            for (int i = 0; i < 6; i += 1) {
                finalVA[i].color = sf::Color{ 128, 128, 128 };
            }

            finalVA[6].color = sf::Color::Red;
            finalVA[7].color = sf::Color::Red;
        }

        window.clear();
        window.draw(finalVA, segmentCount * 2, sf::Lines);
        window.display();

        dt = clock.restart().asMicroseconds();
    }

    return 0;
}