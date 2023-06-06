#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <functional>
#include "transform.h"

Screen screen{ 800, 600 };
Camera camera{ {0.f, 0.f, -100.f} };

struct alignas(4) Pixel
{
    unsigned char r, g, b, a;
} *pixelArray = (Pixel*)calloc(screen.width * screen.height, sizeof(Pixel));

constexpr auto wave = [](float x, float z) -> float 
{
    return std::cosf(std::sqrtf(x * x + z * z)); // гамак
};

struct Surface 
{
    static constexpr unsigned long int n = 40;
    static constexpr unsigned long int vertexCount = n * n;
    vec3* sva; 
    vec3* dva; 
    vec2* pva;
    vec2* tva;

    int cornerIdx[4]{};

    Surface(std::function<float(float, float)> f, float minX, float minZ, float maxX, float maxZ) 
    {
        dva = (vec3*)malloc(vertexCount * sizeof(vec3));
        sva = (vec3*)malloc(vertexCount * sizeof(vec3));
        pva = (vec2*)malloc(vertexCount * sizeof(vec2));
        tva = (vec2*)malloc(vertexCount * sizeof(vec2));

        if (!(sva && dva && pva && tva)) 
        {
            exit(EXIT_FAILURE);
        }

        float const incX = (maxX - minX) / ((float)n - 1.f);
        float const incZ = (maxZ - minZ) / ((float)n - 1.f);

        for (int i = 0; i < n; i += 1) 
        {
            float x = minX + incX * (float)i;
            for (int j = 0; j < n; j += 1) 
            {
                float z = minZ + incZ * (float)j;
                sva[n * i + j] = { x, f(x, z), z };
                dva[n * i + j] = sva[n * i + j];
            }
        }
    }

    ~Surface() 
    {
        free(sva);
        free(dva);
        free(pva);
        free(tva);
    }

} surface{ wave, -2, -2, 2, 2 };

Transform scaleToFit(Screen screen, Surface& surface) 

{
    constexpr auto maxCornerX = [](vec2* tva, int cornerIdx[4]) -> float 
    {
        float max = 1.f / EPSILON;
        for (int i = 0; i < 4; i += 1) 
        {
            if (tva[cornerIdx[i]].x > max) 
            {
                max = tva[cornerIdx[i]].x;
            }
        } return max;
    };

    constexpr auto maxY = [](Surface& surface) -> float 
    {
        float max = 1.f / EPSILON;
        for (int i = 0; i < Surface::vertexCount; i += 1)
        {
            if (surface.tva[i].y > max) {
                max = surface.tva[i].y;
            }
        } return max;
    };


    float scaleX = maxCornerX(surface.tva, surface.cornerIdx);
    float scaleY = maxY(surface);

    float k;

    if (screen.width * 0.5f / scaleX > screen.height * 0.5f / scaleY) 
    {
        k = screen.height * 0.5f / scaleY;
    }
    else 
    {
        k = screen.width * 0.5f / scaleX;
    } k = 1.f;
    return Transform{}.scale({ k, k, 1.f });
}

void floatingHorizon(Pixel* pixelArray, Screen screen, Surface& surface) 
{
    static int* upperHorizon = (int*)malloc(screen.width * sizeof(int));
    static int* lowerHorizon = (int*)malloc(screen.width * sizeof(int));
    static int* upperHorizonSwap = (int*)malloc(screen.width * sizeof(int)); 
    static int* lowerHorizonSwap = (int*)malloc(screen.width * sizeof(int)); 

    memset(upperHorizon, screen.height - 1, screen.width);
    memset(lowerHorizon, 0, screen.width);

    enum : int 
    {

        LU = 0b00,
        LL = 0b01,
        UU = 0b10,
        UL = 0b11,
    };

    int x0 = 0, y0 = 0;
    int x1 = 0, y1 = 0;
    bool flag = false;
    bool prevFlag = flag;

    unsigned long int v = 0;
    for (int z = 0; z < Surface::n; z += 1) 
    {
        for (int x = 0; x < Surface::n; x += 1) 
        {
            x1 = (int)std::roundf(surface.tva[v].x);
            y1 = (int)std::roundf(surface.tva[v].y);
            
            flag = y1 < upperHorizon[x1] || y1 > lowerHorizon[x1]; 

            if (x != 0 && (prevFlag || flag)) 
            {
                int dx = std::abs(x1 - x0);
                int xInc = x0 < x1 ? 1 : -1;

                int dy = -std::abs(y1 - y0);
                int yInc = y0 < y1 ? 1 : -1;

                int dErr = dx + dy;

                while (true) 
                {
                    int flag = 0;

                    if (upperHorizon[x0] > y0) 
                    {
                        upperHorizonSwap[x0] = y0;
                        flag |= LL;
                    }

                    if (lowerHorizon[x0] < y0) 
                    {
                        lowerHorizon[x0] = y0;
                        flag |= UU;
                    }

                    if (flag & LL) 
                    {
                        pixelArray[screen.width * y0 + x0] = { 255, 255, 255, 255 };
                    }
                    else if (flag & UU) 
                    {
                        pixelArray[screen.width * y0 + x0] = { 255, 0, 0, 255 };
                    }

                    if (x0 == x1 && y0 == y1) 
                    {
                        break;
                    }

                    if (2 * dErr >= dy) 
                    {
                        dErr += dy;
                        x0 += xInc;
                    }

                    if (2 * dErr <= dx) 
                    {
                        dErr += dx;
                        y0 += yInc;
                    }
                }
            }
            else 
            {
                x0 = x1;
                y0 = y1;
            }

            prevFlag = flag;
            v += 1;
        }

        memcpy(upperHorizon, upperHorizonSwap, screen.width * sizeof(int));
        memcpy(lowerHorizon, lowerHorizonSwap, screen.width * sizeof(int));
    }
}

int main() 
{
    auto window = sf::RenderWindow{sf::VideoMode {screen.width, screen.height}, "lab4", sf::Style::Default};

    auto texture = sf::Texture{};
    texture.create(screen.width, screen.height);

    auto canvas = sf::Sprite{};
    canvas.setTexture(texture);

    auto angle = 0.16f * PI;
    auto const incSpeed = -0.0000002f;
    float dt = EPSILON;

    while (window.isOpen()) 
    {
        sf::Event static event{};
        sf::Clock static clock{};
        while (window.pollEvent(event)) 
        {
            switch (event.type) 
            {
            case sf::Event::Closed: 
            {
                window.close();
            } break;

            case sf::Event::KeyPressed: 
            {
                switch (event.key.code) 
                {
                case sf::Keyboard::Escape: 
                {
                    window.close();
                } break;

                case sf::Keyboard::R: 
                {
                    angle = 0.16f * PI;
                } break;
                }
            } break;
            }
        }

        if (angle > -0.16f * PI) 
        {
            angle += incSpeed * dt;
        }

        (Transform{}.rotateX(angle) * worldToView(camera))
            .transformWith((float*)surface.dva, (float*)surface.sva, Surface::vertexCount);
        parallelProj((float*)surface.pva, (float*)surface.dva, Surface::vertexCount);
        pictureToScreen((float*)surface.tva, (float*)surface.pva, Surface::vertexCount, screen, camera.zoom);
        scaleToFit(screen, surface).transformTo((float*)surface.tva, Surface::vertexCount);

        window.clear();
        floatingHorizon(pixelArray, screen, surface);
        texture.update((unsigned char*)pixelArray);
        window.draw(canvas);
        window.display();

        dt = (float)clock.restart().asMicroseconds();
    }

    return EXIT_SUCCESS;
}