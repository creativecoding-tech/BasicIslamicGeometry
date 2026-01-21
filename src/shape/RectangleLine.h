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
	std::string label = "";
	RectangleLine(vec2 start,vec2 end,string label);

	// Helper method position
	void setStart(float startX, float startY);
	void setEnd(float endX, float endY);
	void setLabel(std::string label);          // Setter untuk label

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

