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
	float posX = 0;
	float posY = 0;

	CircleShape(float r, std::string label,float posX,float posY);

	void show();
	void hide();
	void update();
	void draw();
	void setLabel(std::string label);
	void setPosition(float x, float y);
private:
	ofTrueTypeFont font;
};

