#pragma once
#include <string>
#include "ofMain.h"
#include "AbstractShape.h"

// namespace alias
using glm::vec2;
class ParallelogramLine : public AbstractShape
{
	public:
		vec2 start;
		vec2 end;
		vec2 intersecCrossLine;  // Intersection point dengan crossline
		std::string label = "";
		ParallelogramLine(vec2 start, vec2 end, vec2 intersecCrossLine, std::string label, float radius);

		// Helper method position
		void setStart(float startX, float startY);
		void setEnd(float endX, float endY);
		void setIntersecCrossLine(vec2 intersec);  // Setter untuk intersection point
		void setLabel(std::string label);          // Setter untuk label

		// Dot visibility control
		void showDot();
		void hideDot();

		// Override addDotsToCache untuk menambahkan Parallelogram dots
		void addDotsToCache(std::vector<DotInfo>& dots) override;

		// Label visibility control
		void showLabel();
		void hideLabel();

		// Override setRadius untuk runtime update
		void setRadius(float r) override;

		// Override pure virtual methods dari AbstractShape
		void update() override;
		void draw() override;

private:
	float radius;         // Radius template
	float originalRadius; // Simpan radius awal untuk proportional scaling
	float totalSegments = 100;  // specific
	bool dotVisible = true;     // Flag untuk dot visibility
	bool labelVisible = true;   // Flag untuk label visibility

};

