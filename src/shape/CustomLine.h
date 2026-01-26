#pragma once

#include "ofMain.h"
using glm::vec2;

/**
 * CustomLine - Garis custom yang bisa dibuat user dengan mouse drag
 * Mendukung:
 * - Garis lurus
 * - Bezier curve dengan parameter curve
 * - Progressive drawing animation
 */
class CustomLine {
public:
	// Constructor
	CustomLine();
	CustomLine(vector<vec2> points, ofColor color, float lineWidth);

	// Main drawing method
	void draw() const;

	// Setters
	void setPoints(const vector<vec2>& points);
	void setColor(ofColor color);
	void setLineWidth(float width);
	void setCurve(float curve);
	void setProgress(float progress);
	void setSpeed(float speed);

	// Getters
	const vector<vec2>& getPoints() const { return points; }
	ofColor getColor() const { return color; }
	float getLineWidth() const { return lineWidth; }
	float getCurve() const { return curve; }
	float getProgress() const { return progress; }
	float getSpeed() const { return speed; }

	// Selection state
	void setSelected(bool selected);
	bool isSelected() const { return selected; }

	// Animation update
	void updateProgress();

private:
	vector<vec2> points;   // 2 titik: start dan end
	ofColor color;
	float lineWidth;
	float curve;          // 0 = lurus, != 0 = bezier curve
	float progress;       // 0.0 - 1.0 untuk progressive drawing
	float speed;          // Kecepatan animasi
	bool selected;        // Selection state untuk visual feedback

	// Helper methods
	void drawStraightLine(vec2 start, vec2 end) const;
	void drawCurvedLine(vec2 start, vec2 end) const;
	vec2 calculateControlPoint(vec2 start, vec2 end) const;
};
