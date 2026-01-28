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
	vec2 radiusDot;  // Posisi dot pada radius (intersection point)

	CrossLine(vec2 start, vec2 end, string label1, string label2, float radius);

	// Helper method position
	void setStart(float startX, float startY);
	void setEnd(float endX, float endY);
	void setRadius(float r) override;  // Override setRadius

	// Label setters
	void setLabel1(string lbl);
	void setLabel2(string lbl);

	// Label visibility control
	void showLabel();
	void hideLabel();

	// Dot visibility control
	void showDot();
	void hideDot();

	// Override addDotsToCache untuk menambahkan CrossLine dots
	void addDotsToCache(std::vector<DotInfo>& dots) override;

	// Override pure virtual methods dari AbstractShape
	void update() override;
	void draw() override;

private:
	float radius;         // Pindah ke private
	float originalRadius; // Simpan radius awal untuk proportional scaling
	int quadrant;   // 0,1,2,3 - Simpan kuadran untuk re-calculate
	float totalSegments = 100;  // CrossLine-specific
	bool labelVisible = true;   // Flag untuk label visibility
	bool dotVisible = true;     // Flag untuk dot visibility
};

