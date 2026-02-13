#pragma once

#include "ofMain.h"
using glm::vec2;

/**
 * CustomLine - Garis custom yang bisa dibuat user dengan mouse drag
 * Mendukung:
 * - Garis lurus
 * - Bezier curve dengan parameter curve
 * - Progressive drawing animation
 * - Axis lock untuk DcustomLine (duplicate lines)
 */
enum class AxisLock {
	NONE,       // Bebas bergerak di X dan Y
	LOCK_X,     // Hanya bisa bergerak di Y (atas bawah)
	LOCK_Y,     // Hanya bisa bergerak di X (kanan kiri)
	LOCK_BOTH   // Tidak bisa gerak sama sekali (fully locked)
};

class CustomLine {
public:
	// Constructor
	CustomLine();
	CustomLine(vector<vec2> points, ofColor color, float lineWidth, std::string label = "");

	// Main drawing method
	void draw() const;

	// Setters
	void setPoints(const vector<vec2>& points);
	void setColor(ofColor color);
	void setLineWidth(float width);
	void setCurve(float curve);
	void setProgress(float progress);
	void setSpeed(float speed);
	void setLabel(const std::string& label);

	// Getters
	const vector<vec2>& getPoints() const { return points; }
	vector<vec2> getSampledPoints() const;  // Ambil semua titik sampling dari curve (100 segments)
	ofColor getColor() const { return color; }
	float getLineWidth() const { return lineWidth; }
	float getCurve() const { return curve; }
	float getProgress() const { return progress; }
	float getSpeed() const { return speed; }
	const std::string& getLabel() const { return label; }

	// Selection state
	void setSelected(bool selected);
	bool isSelected() const { return selected; }

	// Duplicate state
	void setIsDuplicate(bool duplicate) { isDuplicate = duplicate; }
	bool getIsDuplicate() const { return isDuplicate; }

	// Axis lock state (untuk DcustomLine)
	void setAxisLock(AxisLock lock) { axisLock = lock; }
	AxisLock getAxisLock() const { return axisLock; }

	// Animation update
	void updateProgress(float deltaTime = 0.016f);

	// Helper for curve tracking
	vec2 getPointAt(float t) const;        // Get point at t (0.0 - 1.0)
	float getClosestT(vec2 point) const;   // Get closest t for a point
	float getApproxLength() const;         // Get approximate length of the line/curve

private:
	vector<vec2> points;   // 2 titik: start dan end
	ofColor color;
	float lineWidth;
	float curve;          // 0 = lurus, != 0 = bezier curve
	float progress;       // 0.0 - 1.0 untuk progressive drawing
	float speed;          // Kecepatan animasi
	bool selected;        // Selection state untuk visual feedback
	std::string label;    // Label untuk customLine (cth: "customLine0", "DcustomLine0")
	bool isDuplicate;     // True jika ini duplicate line (hasil dari duplicateLineR180)
	AxisLock axisLock;    // Axis lock untuk DcustomLine (NONE, LOCK_X, LOCK_Y)

	// Helper methods
	void drawStraightLine(vec2 start, vec2 end) const;
	void drawCurvedLine(vec2 start, vec2 end) const;
	vec2 calculateControlPoint(vec2 start, vec2 end) const;
};
