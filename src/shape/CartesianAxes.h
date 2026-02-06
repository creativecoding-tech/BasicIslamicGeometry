#pragma once
#include "AbstractShape.h"

class CartesianAxes : public AbstractShape
{
public:
	CartesianAxes(float r);

	// Override pure virtual methods dari AbstractShape
	void update(float deltaTime = 0.016f) override;
	void draw() override;
	void setRadius(float r) override;  // Override setRadius

	// Override isComplete untuk custom logic
	bool isComplete() override;

	// Label visibility control
	void showLabel();
	void hideLabel();

private:
	float radius;  // Pindah ke private
	float maxScale = 2.5;  // Axes-specific
	bool labelVisible = true;  // Flag untuk label visibility
};

