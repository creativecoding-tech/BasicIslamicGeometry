#include "PolygonShape.h"

//--------------------------------------------------------------
PolygonShape::PolygonShape()
	: vertices(), fillColor(ofColor(255, 0, 0, 150)), selected(false) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color)
	: vertices(verts), fillColor(color), selected(false) {}

//--------------------------------------------------------------
void PolygonShape::draw() const {
	ofPushStyle();
	ofFill();  // Hanya fill, TANPA outline!
	ofSetColor(selected ? ofColor(255, 0, 0) : fillColor);  // Merah jika selected
	ofBeginShape();
	for (auto& v : vertices) {
		ofVertex(v.x, v.y);
	}
	ofEndShape(true);  // Close shape
	ofPopStyle();
	// NO OUTLINE! Cuma fill!
}

//--------------------------------------------------------------
void PolygonShape::setColor(ofColor color) {
	fillColor = color;
}

//--------------------------------------------------------------
void PolygonShape::setSelected(bool sel) {
	selected = sel;
}

//--------------------------------------------------------------
ofColor PolygonShape::getColor() const {
	return fillColor;
}

//--------------------------------------------------------------
bool PolygonShape::isSelected() const {
	return selected;
}

//--------------------------------------------------------------
const vector<vec2>& PolygonShape::getVertices() const {
	return vertices;
}
