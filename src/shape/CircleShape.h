#pragma once
#include <string>
#include <ofMain.h>
class CircleShape
{
public:
	float radius;
	float circleProgress = 0;
	float totalSegments = 100;
	float circleSpeed = .5f;
	bool showing = false;
	std::string label = "";

	CircleShape(float r, std::string label);

	void show();
	void hide();
	void update();
	void draw();
	void setLabel(std::string label);
private:
	ofTrueTypeFont font;
};

