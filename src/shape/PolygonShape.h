#pragma once

#include "ofMain.h"
using glm::vec2;

/**
 * PolygonShape - Polygon dengan fill TANPA outline (invisible edge)
 * Digunakan untuk membuat area berwarna tanpa garis tepi
 */
class PolygonShape {
public:
	// Constructor
	PolygonShape();
	PolygonShape(vector<vec2> verts, ofColor color);

	// Main drawing method - draw fill ONLY, no outline
	void draw() const;

	// Setters
	void setColor(ofColor color);
	void setSelected(bool sel);

	// Getters
	ofColor getColor() const;
	bool isSelected() const;
	const vector<vec2>& getVertices() const;

private:
	vector<vec2> vertices;
	ofColor fillColor;
	bool selected;
};
