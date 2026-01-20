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
		ParallelogramLine(vec2 start, vec2 end, vec2 intersecCrossLine);

		// Helper method position
		void setStart(float startX, float startY);
		void setEnd(float endX, float endY);
		void setIntersecCrossLine(vec2 intersec);  // Setter untuk intersection point

		// Override pure virtual methods dari AbstractShape
		void update() override;
		void draw() override;

private:
	float totalSegments = 100;  // specific

};

