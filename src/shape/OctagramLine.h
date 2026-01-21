#pragma once
#include <string>
#include "ofMain.h"
#include "AbstractShape.h"
// namespace alias
using glm::vec2;
using std::string;
class OctagramLine : public AbstractShape
{
public:
	vec2 start;
	vec2 end;
	std::string label = "";
	OctagramLine(vec2 start, vec2 end, string label);
	// Helper method position
	void setStart(float startX, float startY);
	void setEnd(float endX, float endY);

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

