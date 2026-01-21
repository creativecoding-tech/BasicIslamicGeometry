#pragma once
#include <string>
#include "ofMain.h"
#include "AbstractShape.h"
// namespace alias
using glm::vec2;
using std::string;
class RectangleLine : public AbstractShape
{
public:
	vec2 start;
	vec2 end;
	vec2 intersec1;  // Intersection pertama (F→G ∩ C→E)
	vec2 intersec2;  // Intersection kedua (F→G ∩ B→E)
	std::string label1 = "";  // Label untuk dot pertama (R)
	std::string label2 = "";  // Label untuk dot kedua (S)
	RectangleLine(vec2 start, vec2 end, vec2 intersec1, vec2 intersec2, string label1, string label2);

	// Helper method position
	void setStart(float startX, float startY);
	void setEnd(float endX, float endY);
	void setIntersec1(vec2 intersec1);
	void setIntersec2(vec2 intersec2);

	// Dot visibility control
	void showDot();
	void hideDot();

	// Label visibility control
	void showLabel();
	void hideLabel();

	// Override pure virtual methods dari AbstractShape
	void update() override;
	void draw() override;

private:
	float totalSegments = 100;  // specific
	bool dotVisible = true;     // Flag untuk dot visibility
	bool labelVisible = true;   // Flag untuk label visibility
};

