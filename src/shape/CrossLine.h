#pragma once
#include <string>
#include "ofMain.h"
#include "AbstractShape.h"

// namespace alias
using glm::vec2;
using std::string;

class CrossLine : public AbstractShape
{
public:
	vec2 start;
	vec2 end;
	string label1 = "";
	string label2 = "";

	CrossLine(vec2 start, vec2 end, string label1, string label2);

	// Helper method position
	void setStart(float startX, float startY);
	void setEnd(float endX, float endY);

	// Label setters
	void setLabel1(string lbl);
	void setLabel2(string lbl);

	// Override pure virtual methods dari AbstractShape
	void update() override;
	void draw() override;

private:
	float totalSegments = 100;  // CrossLine-specific
};

