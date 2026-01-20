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
		ParallelogramLine(vec2 start,vec2 end);

		// Helper method position
		void setStart(float startX, float startY);
		void setEnd(float endX, float endY);

		// Override pure virtual methods dari AbstractShape
		void update() override;
		void draw() override;

private:
	float totalSegments = 100;  // specific

};

