#include "PolygonShape.h"
#include "../anim/FadeInAnimation.h"

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
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color, int index, std::unique_ptr<FadeInAnimation> anim)
	: vertices(verts), fillColor(color), selected(false), index(index), animation(std::move(anim)) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(const PolygonShape& other)
	: vertices(other.vertices)
	, fillColor(other.fillColor)
	, selected(other.selected)
	, index(other.index)
	, animation(nullptr) {}

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
// Move constructor - transfer animation ownership
PolygonShape::PolygonShape(PolygonShape&& other) noexcept
	: vertices(std::move(other.vertices))
	, fillColor(other.fillColor)
	, selected(other.selected)
	, index(other.index)
	, animation(std::move(other.animation)) {}

//--------------------------------------------------------------
// Move assignment operator - transfer animation ownership
PolygonShape& PolygonShape::operator=(PolygonShape&& other) noexcept {
	if (this != &other) {
		vertices = std::move(other.vertices);
		fillColor = other.fillColor;
		selected = other.selected;
		index = other.index;
		animation = std::move(other.animation);
	}
	return *this;
}

//--------------------------------------------------------------
void PolygonShape::draw() const {
	ofPushStyle();
	ofFill();

	// Gunakan animation alpha untuk fade-in
	if (animation) {
		ofColor animatedColor = fillColor;

		// DEBUG: Cek progress
		float progress = animation->getProgress();

		// Kalau progress masih 0, alpha harus 0 (BENAR-BENAR HIDDEN)
		if (progress == 0.0f) {
			animatedColor.a = 0;  // FORCE alpha 0!
		} else {
			// Progress > 0, pakai current alpha
			float currentAlpha = animation->getCurrentAlpha();
			animatedColor.a = static_cast<int>(currentAlpha);
		}

		ofSetColor(animatedColor);
	} else {
		// Tidak ada animation, pakai fillColor asli
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
bool PolygonShape::hasAnimation() const {
	return animation != nullptr;
}

//--------------------------------------------------------------
bool PolygonShape::isAnimationComplete() const {
	if (animation) {
		return animation->isComplete();
	}
	return true;  // Kalau tidak ada animation, dianggap complete
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
void PolygonShape::setVertices(const vector<vec2>& verts) {
	vertices = verts;
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
