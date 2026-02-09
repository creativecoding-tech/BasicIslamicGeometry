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
	vec2 intersec1;  // Intersection pertama
	vec2 intersec2;  // Intersection kedua
	std::string label1 = "";  // Label untuk dot pertama
	std::string label2 = "";  // Label untuk dot kedua
	RectangleLine(vec2 start, vec2 end, vec2 intersec1, vec2 intersec2, string label1, string label2, float radius);

	// Helper method position
	void setStart(float startX, float startY);
	void setEnd(float endX, float endY);
	void setIntersec1(vec2 intersec1);
	void setIntersec2(vec2 intersec2);

	// Dot visibility control
	void showDot();
	void hideDot();

	// Override addDotsToCache untuk menambahkan Rectangle dots
	void addDotsToCache(std::vector<DotInfo>& dots) override;

	// Label visibility control
	void showLabel();
	void hideLabel();

	// Override setRadius untuk runtime update
	void setRadius(float r) override;

	// Override pure virtual methods dari AbstractShape
	void update(float deltaTime = 0.016f) override;
	void draw() override;

private:
	float radius;         // Radius template
	float originalRadius; // Simpan radius awal untuk proportional scaling
	float totalSegments = 100;  // specific
	bool dotVisible = true;     // Flag untuk dot visibility
	bool labelVisible = true;   // Flag untuk label visibility
};

