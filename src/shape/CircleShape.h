#pragma once
#include <string>
#include "ofMain.h"
#include "AbstractShape.h"

class CircleShape : public AbstractShape
{
public:
	float radius;
	std::string label = "";
	float posX = 0;
	float posY = 0;
	CircleShape(float r, std::string label,float posX,float posY);

	// Override pure virtual methods dari AbstractShape
	void update() override;
	void draw() override;

	void setLabel(std::string label);
	void setPosition(float x, float y);

private:
	float totalSegments = 100;  // Circle-specific
};

