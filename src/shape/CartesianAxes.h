#pragma once
#include "AbstractShape.h"

class CartesianAxes : public AbstractShape
{
public:
	float radius;
	CartesianAxes(float r);

	// Override pure virtual methods dari AbstractShape
	void update() override;
	void draw() override;

	// Override isComplete untuk custom logic
	bool isComplete() override;

private:
	float maxScale = 2.5;  // Axes-specific
};

