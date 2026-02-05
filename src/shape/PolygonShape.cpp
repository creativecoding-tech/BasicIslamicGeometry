#include "PolygonShape.h"
#include "../anim/FadeInAnimation.h"
#include "../anim/WobbleAnimation.h"
#include "../anim/FillAnimation.h"

//--------------------------------------------------------------
PolygonShape::PolygonShape()
	: vertices(), fillColor(ofColor(255, 0, 0, 150)), selected(false), index(-1), animation(nullptr),
	  minY(0.0f), maxY(0.0f), currentWaterY(0.0f) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color)
	: vertices(verts), fillColor(color), selected(false), index(-1), animation(nullptr),
	  minY(0.0f), maxY(0.0f), currentWaterY(0.0f) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color, int idx)
	: vertices(verts), fillColor(color), selected(false), index(idx), animation(nullptr),
	  minY(0.0f), maxY(0.0f), currentWaterY(0.0f) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color, int index, std::unique_ptr<AbstractAnimation> anim)
	: vertices(verts), fillColor(color), selected(false), index(index), animation(std::move(anim)),
	  minY(0.0f), maxY(0.0f), currentWaterY(0.0f) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(const PolygonShape& other)
	: vertices(other.vertices)
	, fillColor(other.fillColor)
	, selected(other.selected)
	, index(other.index)
	, animation(nullptr)
	, minY(0.0f), maxY(0.0f), currentWaterY(0.0f) {}

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
	, animation(std::move(other.animation))
	, minY(0.0f), maxY(0.0f), currentWaterY(0.0f) {}

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

	// Cek tipe animation dan apply effect yang sesuai
	if (animation) {
		ofColor animatedColor = fillColor;

		// Cek FadeInAnimation
		if (auto* fadeAnim = dynamic_cast<FadeInAnimation*>(animation.get())) {
			float progress = animation->getProgress();

			// Kalau progress masih 0, alpha harus 0 (BENAR-BENAR HIDDEN)
			if (progress == 0.0f) {
				animatedColor.a = 0;  // FORCE alpha 0!
			} else {
				// Progress > 0, pakai current alpha
				float currentAlpha = fadeAnim->getCurrentAlpha();
				animatedColor.a = static_cast<int>(currentAlpha);
			}

			ofSetColor(animatedColor);

			// Gambar polygon tanpa wobble
			ofBeginShape();
			for (auto& v : vertices) {
				ofVertex(v.x, v.y);
			}
			ofEndShape(true);
		}
		// Cek WobbleAnimation
		else if (auto* wobbleAnim = dynamic_cast<WobbleAnimation*>(animation.get())) {
			ofSetColor(animatedColor);

			// Gambar polygon dengan wobble effect pada vertices
			glm::vec2 offset = wobbleAnim->getWobbleOffset();

			ofBeginShape();
			for (auto& v : vertices) {
				// Setiap vertex diberi offset yang sama (seluruh polygon bergoyang)
				ofVertex(v.x + offset.x, v.y + offset.y);
			}
			ofEndShape(true);
		}
		// Cek FillAnimation
		else if (auto* fillAnim = dynamic_cast<FillAnimation*>(animation.get())) {
			// Pixel-by-pixel fill dengan wave boundary
			// Area di bawah wave = terwarnai solid
			// Area di atas wave = tidak terwarnai

			// Hitung bounding box
			float minX = vertices[0].x, maxX = vertices[0].x;
			for (const auto& v : vertices) {
				if (v.x < minX) minX = v.x;
				if (v.x > maxX) maxX = v.x;
			}

			// Buat ofPolyline untuk cek inside
			ofPolyline poly;
			for (const auto& v : vertices) {
				poly.addVertex(v.x, v.y);
			}
			poly.close();

			// Gambar pixel by pixel dengan anti-aliasing
			// Step size untuk performance
			float step = 2.0f;
			// Anti-aliasing margin (lebar gradient boundary)
			float aaMargin = 4.0f;

			for (float x = minX; x <= maxX; x += step) {
				for (float y = minY; y <= maxY; y += step) {
					// Cek apakah point di dalam polygon
					if (poly.inside(x, y)) {
						// Cek apakah point DI BAWAH wave
						float waveOffset = fillAnim->getWaveOffset(x);
						float effectiveWaterY = currentWaterY + waveOffset;

						// Hitung distance dari point ke wave surface
						float distFromWater = y - effectiveWaterY;

						if (distFromWater >= 0) {
							// Di bawah wave
							if (distFromWater >= aaMargin) {
								// Jauh dari boundary, solid color
								ofSetColor(fillColor);
								ofDrawRectangle(x, y, step, step);
							} else {
								// Dekat boundary, anti-aliasing dengan alpha gradient
								float aaRatio = distFromWater / aaMargin;  // 0.0 = di boundary, 1.0 = jauh
								aaRatio = ofClamp(aaRatio, 0.0f, 1.0f);
								int aaAlpha = static_cast<int>(fillColor.a * aaRatio);
								ofSetColor(fillColor.r, fillColor.g, fillColor.b, aaAlpha);
								ofDrawRectangle(x, y, step, step);
							}
						}
						// Di atas wave: tidak gambar apa-apa
					}
				}
			}
		}
		// Unknown animation type - fallback ke no animation
		else {
			ofSetColor(fillColor);
			ofBeginShape();
			for (auto& v : vertices) {
				ofVertex(v.x, v.y);
			}
			ofEndShape(true);
		}
	} else {
		// Tidak ada animation, pakai fillColor asli
		ofSetColor(fillColor);

		ofBeginShape();
		for (auto& v : vertices) {
			ofVertex(v.x, v.y);
		}
		ofEndShape(true);
	}

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
		ofSetColor(0, 0, 0);
		ofDrawBitmapString("Polygon" + ofToString(index), centroid.x - 10,centroid.y);
	}

	ofPopStyle();
}

//--------------------------------------------------------------
void PolygonShape::update() {
	// Update animation jika ada
	if (animation) {
		animation->update();

		// Jika FillAnimation, hitung fill state (minY, maxY, currentWaterY)
		if (auto* fillAnim = dynamic_cast<FillAnimation*>(animation.get())) {
			// Hitung bounding box Y (minY = atas, maxY = bawah)
			minY = vertices[0].y;
			maxY = vertices[0].y;
			for (const auto& v : vertices) {
				if (v.y < minY) minY = v.y;
				if (v.y > maxY) maxY = v.y;
			}

			// Hitung current water Y (naik dari bawah ke atas)
			float waterLevel = fillAnim->getWaterLevel();
			currentWaterY = maxY - (maxY - minY) * waterLevel;
		}
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
