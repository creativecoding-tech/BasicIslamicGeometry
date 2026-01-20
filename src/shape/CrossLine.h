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
	float radius;  // Radius untuk dot position

	CrossLine(vec2 start, vec2 end, string label1, string label2, float radius);

	// Helper method position
	void setStart(float startX, float startY);
	void setEnd(float endX, float endY);

	// Label setters
	void setLabel1(string lbl);
	void setLabel2(string lbl);

	// Label visibility control
	void showLabel();
	void hideLabel();

	// Override pure virtual methods dari AbstractShape
	void update() override;
	void draw() override;

private:
	float totalSegments = 100;  // CrossLine-specific
	bool labelVisible = true;   // Flag untuk label visibility
};

