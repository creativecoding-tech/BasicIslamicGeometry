#pragma once
#include <string>
#include <optional>
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
	std::optional<vec2> nextPoint;  // Optional titik lanjutan (jika ada, gambar 2 garis)
	std::string label = "";
	OctagramLine(vec2 start, vec2 end, std::optional<vec2> nextPoint, string label);
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

	// Override isComplete untuk ngecek kedua progress (main + extension)
	bool isComplete() override;

	// Mode control untuk animation
	void setSequentialMode(bool sequential) { isSequentialMode = sequential; }

private:
	float totalSegments = 100;  // specific
	float extensionProgress = 0;  // Progress untuk extension line (phase 2)
	bool isSequentialMode = false;  // Flag untuk sequential vs paralel mode
	bool dotVisible = true;     // Flag untuk dot visibility
	bool labelVisible = true;   // Flag untuk label visibility
};

