#pragma once

#include "ofMain.h"
#include <memory>
#include "../anim/AbstractAnimation.h"
using glm::vec2;

/**
 * PolygonShape - Polygon dengan fill TANPA outline (invisible edge)
 * Digunakan untuk membuat area berwarna tanpa garis tepi
 * Mendukung animation system (FadeInAnimation, WobbleAnimation, dll)
 */
class PolygonShape {
public:
	// Constructor
	PolygonShape();
	PolygonShape(vector<vec2> verts, ofColor color);
	PolygonShape(vector<vec2> verts, ofColor color, int index);  // Dengan index untuk label
	PolygonShape(vector<vec2> verts, ofColor color, int index, std::unique_ptr<AbstractAnimation> anim);  // Dengan animation (untuk load dari file)

	// Copy constructor (animation tidak dicopy)
	PolygonShape(const PolygonShape& other);

	// Copy assignment operator (animation tidak dicopy)
	PolygonShape& operator=(const PolygonShape& other);

	// Move constructor (transfer animation ownership)
	PolygonShape(PolygonShape&& other) noexcept;

	// Move assignment operator (transfer animation ownership)
	PolygonShape& operator=(PolygonShape&& other) noexcept;

	// Main drawing method - draw fill ONLY, no outline
	void draw() const;

	// Animation
	void update(float deltaTime = 0.016f);  // Update animation progress
	bool hasAnimation() const;  // Cek apakah punya animation
	bool isAnimationComplete() const;  // Cek apakah animation sudah selesai

	// Setters
	void setColor(ofColor color);
	void setSelected(bool sel);
	void setVertices(const vector<vec2>& verts);  // Set vertices (untuk scaling)

	// Getters
	ofColor getColor() const;
	bool isSelected() const;
	const vector<vec2>& getVertices() const;
	int getIndex() const { return index; }  // Get index polygon

	// Utils
	bool containsPoint(vec2 point) const;  // Cek apakah point ada di dalam polygon

private:
	vector<vec2> vertices;
	ofColor fillColor;
	bool selected;
	int index;  // Index polygon untuk label
	std::unique_ptr<AbstractAnimation> animation;  // Animation system (optional)

	// Fill state (dihitung di update(), dipakai di draw())
	mutable float minY;                      // Y minimum (atas)
	mutable float maxY;                      // Y maksimum (bawah)
	mutable float currentWaterY;             // Posisi Y air saat ini
};
