#include "PolygonShape.h"

//--------------------------------------------------------------
PolygonShape::PolygonShape()
	: vertices(), fillColor(ofColor(255, 0, 0, 150)), selected(false), index(-1) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color)
	: vertices(verts), fillColor(color), selected(false), index(-1) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color, int idx)
	: vertices(verts), fillColor(color), selected(false), index(idx) {}

//--------------------------------------------------------------
void PolygonShape::draw() const {
	ofPushStyle();
	ofFill();
	ofSetColor(fillColor);
	ofBeginShape();
	for (auto& v : vertices) {
		ofVertex(v.x, v.y);
	}
	ofEndShape(true);

	// Jika selected, gambar label di tengah polygon
	if (selected && index >= 0) {
		// Hitung centroid (titik tengah)
		vec2 centroid(0, 0);
		for (auto& v : vertices) {
			centroid += v;
		}
		centroid /= vertices.size();

		// Gambar label "P[index]"
		ofNoFill();
		ofSetColor(255, 255, 255);  // Putih
		ofDrawBitmapString("Polygon" + ofToString(index), centroid.x - 10,centroid.y);
	}

	ofPopStyle();
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

//--------------------------------------------------------------
bool PolygonShape::containsPoint(vec2 point) const {
	if (vertices.size() < 3) return false;

	// Gunakan ofPolyline untuk cek apakah point di dalam polygon
	ofPolyline poly;
	for (auto& v : vertices) {
		poly.addVertex(v.x, v.y);
	}

	return poly.inside(point.x, point.y);
}
