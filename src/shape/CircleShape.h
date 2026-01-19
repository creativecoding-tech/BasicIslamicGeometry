#pragma once
class CircleShape
{
public:
	float radius;
	float circleProgress = 0;
	float totalSegments = 100;
	float circleSpeed = .5f;
	bool showing = false;

	CircleShape(float r);

	void show();
	void hide();
	void update();
	void draw();


};

