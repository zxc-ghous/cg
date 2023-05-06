#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "object.h"

class Scene
{
public:
	Scene();
	~Scene();

	bool isOpen();

	void pollEvents(bool& restart);

	void render();

	void addObject(Object* obj);

private:
	std::vector<Object*> objects;

	sf::RenderWindow* window;
};

Scene::Scene()
{
	window = new sf::RenderWindow(sf::VideoMode(800, 800), "leaf falling", sf::Style::Default);
}

Scene::~Scene()
{
	if (window)
		delete window;
}

bool Scene::isOpen()
{
	return window->isOpen();
}

void Scene::pollEvents(bool& restart)
{
	restart = false;

	sf::Event event;
	while (window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window->close();
		}

		if (event.type == sf::Event::KeyPressed)
			if (event.key.code == sf::Keyboard::R)
				restart = true;
	}
}

void Scene::render()
{
	window->clear(sf::Color::Blue);
	//тут спрайт драв
	sf::RectangleShape ground(sf::Vector2f(800.f, 300.f));
	ground.setFillColor(sf::Color(128, 64, 48));
	ground.setPosition(0.f, 500.f);
	sf::CircleShape circle(150.f);
	circle.setFillColor(sf::Color(1, 50, 32));
	circle.setPosition(400.f, -50.f);
	window->draw(ground);


	for (int i = 0; i < objects.size(); i++)
		objects[i]->render(*window);
	window->draw(circle);
	window->display();
}

void Scene::addObject(Object* obj)
{
	objects.push_back(obj);
}