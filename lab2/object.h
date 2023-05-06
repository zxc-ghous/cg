#pragma once
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "transform.h"

class Object
{
public:
	Object();
	~Object();

	Object(std::vector<std::vector<vec>> p, int r_, int g_, int b_);
	Object(const Object& other);
	void operator=(const Object& other);
	Object(Object&& other) noexcept;
	void operator=(Object&& other) noexcept;

	void rotate(float angle);
	void translate(vec translation);
	void scaling(float coeff);

	void render(sf::RenderWindow& window);
private:
	int r;
	int g;
	int b;
	matrix translation;
	matrix rotation;
	matrix scale;
	std::vector<std::vector<vec>> shapes;
};

Object::Object()
{
	translation = matrix();
	rotation = matrix();
	scale = matrix();
	shapes = std::vector<std::vector<vec>>(0);
}

Object::~Object()
{
}

Object::Object(std::vector<std::vector<vec>> p, int r_, int g_, int b_)
{
	translation = matrix();
	rotation = matrix();
	scale = matrix();
	shapes = p;
	r = r_;
	g = g_;
	b = b_;
}

Object::Object(const Object& other)
{
	translation = other.translation;
	rotation = other.rotation;
	scale = other.scale;
	shapes = other.shapes;
}

void Object::operator=(const Object& other)
{
	if (this == &other)
		return;

	translation = other.translation;
	rotation = other.rotation;
	scale = other.scale;
	shapes = other.shapes;
}

Object::Object(Object&& other) noexcept
{
	translation = std::move(other.translation);
	rotation = std::move(other.rotation);
	scale = std::move(other.scale);
	shapes = std::move(other.shapes);
}

void Object::operator=(Object&& other) noexcept
{
	if (this == &other)
		return;

	translation = other.translation;
	rotation = other.rotation;
	scale = other.scale;
	shapes = other.shapes;
}

void Object::rotate(float angle)
{
	matrix addition;
	float radians = angle * 3.14 / 180.0f;
	addition.mat[0][0] = cosf(radians);
	addition.mat[0][1] = sinf(radians);
	addition.mat[1][0] = -sinf(radians);
	addition.mat[1][1] = cosf(radians);

	rotation *= addition;
}

void Object::translate(vec translation)
{
	matrix addition;
	addition.mat[0][2] = translation.x;
	addition.mat[1][2] = translation.y;

	this->translation *= addition;
}

void Object::scaling(float coeff)
{
	matrix addition;
	addition.mat[0][0] = coeff;
	addition.mat[1][1] = coeff;

	scale *= addition;
}

void Object::render(sf::RenderWindow& window)
{
	for (int i = 0; i < shapes.size(); i++)
	{
		sf::ConvexShape shape;
		shape.setPointCount(shapes[i].size());

		for (int j = 0; j < shapes[i].size(); j++)
		{
			vec new_point = shapes[i][j];
			new_point = scale * new_point;
			new_point = rotation * new_point;
			new_point = translation * new_point;

			shape.setPoint(j, sf::Vector2f(new_point.x, new_point.y));
		}

		shape.setFillColor(sf::Color(r, g, b));

		window.draw(shape);
	}
}