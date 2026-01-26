#include "PolygonShape.h"

//--------------------------------------------------------------
PolygonShape::PolygonShape()
	: vertices(), fillColor(ofColor(255, 0, 0, 150)), selected(false), index(-1), animation(nullptr) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color)
	: vertices(verts), fillColor(color), selected(false), index(-1), animation(nullptr) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color, int idx)
	: vertices(verts), fillColor(color), selected(false), index(idx), animation(nullptr) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(const PolygonShape& other)
	: vertices(other.vertices)
	, fillColor(other.fillColor)
	, selected(other.selected)
	, index(other.index)
	, animation(nullptr) {  // Animation tidak dicopy (reset ke nullptr)
}

//--------------------------------------------------------------
PolygonShape& PolygonShape::operator=(const PolygonShape& other) {
	if (this != &other) {
		vertices = other.vertices;
		fillColor = other.fillColor;
		selected = other.selected;
		index = other.index;
		animation = nullptr;  // Animation tidak dicopy (reset ke nullptr)
	}
	return *this;
}

//--------------------------------------------------------------
void PolygonShape::draw() const {
	ofPushStyle();
	ofFill();

	// Gunakan animation alpha jika ada, otherwise pakai original alpha
	if (animation && !animation->isComplete()) {
		// Ambil current alpha dari animation
		float currentAlpha = animation->getCurrentAlpha();
		ofColor animatedColor = fillColor;
		animatedColor.a = static_cast<int>(currentAlpha);
		ofSetColor(animatedColor);
	} else {
		ofSetColor(fillColor);
	}

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

		// Gambar label "Polygon[index]"
		ofNoFill();
		ofSetColor(255, 255, 255);  // Putih
		ofDrawBitmapString("Polygon" + ofToString(index), centroid.x - 10,centroid.y);
	}

	ofPopStyle();
}

//--------------------------------------------------------------
void PolygonShape::update() {
	// Update animation jika ada
	if (animation) {
		animation->update();
	}
}

//--------------------------------------------------------------
void PolygonShape::setAnimation(std::unique_ptr<FadeInAnimation> anim) {
	animation = std::move(anim);
}

//--------------------------------------------------------------
bool PolygonShape::hasAnimation() const {
	return animation != nullptr;
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
