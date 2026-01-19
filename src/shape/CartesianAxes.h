#pragma once
class CartesianAxes
{
public:
	float radius;
	float progress = 0;
	float maxScale = 2.5;
	float speed = 0.02;
	bool showing = true;
	CartesianAxes(float r);
	void show();
	void hide();
	void update();
	void draw();
	bool isComplete();
};

