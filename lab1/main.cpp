#include <SFML/Graphics.hpp>
#include "scene.h"
#include <iostream>

int main()
{
	Scene my_scene;

	std::vector<std::vector<vec>> shapes;
	std::vector<vec> points;
	//tree
	points.clear();
	shapes.clear();
	points.push_back(vec(450, 500, 1));
	points.push_back(vec(500, 450, 1));
	points.push_back(vec(600, 450, 1));
	points.push_back(vec(650, 500, 1));
	shapes.push_back(points);
	points.clear();
	points.push_back(vec(500, 450, 1));
	points.push_back(vec(500, 100, 1));
	points.push_back(vec(600, 100, 1));
	points.push_back(vec(600, 450, 1));
	shapes.push_back(points);
	points.clear();
	Object* tree = new Object(shapes, 150, 75, 0);
	my_scene.addObject(tree);

	//leaf
	points.clear();
	shapes.clear();
	points.push_back(vec(-50, 0, 1));
	points.push_back(vec(0, 25, 1));
	points.push_back(vec(25, 0, 1));
	points.push_back(vec(0, -25, 1));
	shapes.push_back(points);
	points.clear();
	points.push_back(vec(0,-5,1));
	points.push_back(vec(35,-5,1));
	points.push_back(vec(35,5,1));
	points.push_back(vec(0,5,1));
	shapes.push_back(points);
	Object* obj = new Object(shapes,0,255,0);
	my_scene.addObject(obj);


	//obj->translate(vec(400, 200, 1));
	obj->rotate(45);

	float angle = 55.0f;
	int dir = 1;
	float r = 100.0f;
	float scale_coef = 1.00006;
	while (my_scene.isOpen())
	{
		bool isHit = false;
		my_scene.pollEvents(isHit);

		float x = cosf((angle) * 3.14 / 180.0f) * r;
		float y = sinf((angle) * 3.14 / 180.0f) * r;

		obj->translate(vec(x, y, 1));
		obj->translate(vec(500, 0, 1));
		obj->rotate(0.05f*(-dir));
		obj->scaling(scale_coef);
		my_scene.render();

		angle += 0.05f * dir;
		r += 0.05f;
		obj->translate(vec(-500, 0, 1));
		obj->translate(vec(-x, -y, 1));

		if (angle < 55 || angle>125)
		{
			dir *= -1;
		}
		if (r > 550)
		{
			std::cout << "press any key to exit";
			std::cin >> isHit;
			break;
		}
	}
	return 0;
}