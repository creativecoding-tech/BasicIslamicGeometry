#include "PolygonShape.h"
#include "../anim/FadeInAnimation.h"
#include "../anim/WobbleAnimation.h"
#include "../anim/FillAnimation.h"

//--------------------------------------------------------------
PolygonShape::PolygonShape()
	: vertices(), fillColor(ofColor(255, 0, 0, 150)), selected(false), index(-1), animation(nullptr),
	  minY(0.0f), maxY(0.0f), currentWaterY(0.0f), shaderLoaded(false), fboAllocated(false),
	  lastFboWidth(0), lastFboHeight(0) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color)
	: vertices(verts), fillColor(color), selected(false), index(-1), animation(nullptr),
	  minY(0.0f), maxY(0.0f), currentWaterY(0.0f), shaderLoaded(false), fboAllocated(false),
	  lastFboWidth(0), lastFboHeight(0) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color, int idx)
	: vertices(verts), fillColor(color), selected(false), index(idx), animation(nullptr),
	  minY(0.0f), maxY(0.0f), currentWaterY(0.0f), shaderLoaded(false), fboAllocated(false),
	  lastFboWidth(0), lastFboHeight(0) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color, int index, std::unique_ptr<AbstractAnimation> anim)
	: vertices(verts), fillColor(color), selected(false), index(index), animation(std::move(anim)),
	  minY(0.0f), maxY(0.0f), currentWaterY(0.0f), shaderLoaded(false), fboAllocated(false),
	  lastFboWidth(0), lastFboHeight(0) {}

//--------------------------------------------------------------
PolygonShape::PolygonShape(const PolygonShape& other)
	: vertices(other.vertices)
	, fillColor(other.fillColor)
	, selected(other.selected)
	, index(other.index)
	, animation(nullptr)
	, minY(0.0f), maxY(0.0f), currentWaterY(0.0f), shaderLoaded(false), fboAllocated(false),
	  lastFboWidth(0), lastFboHeight(0) {}

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
	, minY(0.0f), maxY(0.0f), currentWaterY(0.0f), shaderLoaded(false), fboAllocated(false),
	  lastFboWidth(0), lastFboHeight(0) {}

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

	// GLOBAL shader untuk basic polygon rendering (semua polygon kecuali Wave Fill)
	static ofShader globalPolygonShader;
	static bool globalPolygonShaderLoaded = false;

	// Load shader sekali saja
	if (!globalPolygonShaderLoaded) {
		globalPolygonShader.load("shaders/polygon.vert", "shaders/polygon.frag");
		globalPolygonShaderLoaded = true;
	}

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

			// Render dengan shader
			globalPolygonShader.begin();
			globalPolygonShader.setUniform4f("color",
				animatedColor.r / 255.0f,
				animatedColor.g / 255.0f,
				animatedColor.b / 255.0f,
				animatedColor.a / 255.0f);

			// Draw polygon mesh
			ofMesh polygonMesh;
			for (const auto& v : vertices) {
				polygonMesh.addVertex(ofVec3f(v.x, v.y, 0.0f));
			}
			polygonMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
			polygonMesh.draw();

			globalPolygonShader.end();
		}
		// Cek WobbleAnimation
		else if (auto* wobbleAnim = dynamic_cast<WobbleAnimation*>(animation.get())) {
			// Gambar polygon dengan wobble effect pada vertices
			glm::vec2 offset = wobbleAnim->getWobbleOffset();

			// Render dengan shader
			globalPolygonShader.begin();
			globalPolygonShader.setUniform4f("color",
				fillColor.r / 255.0f,
				fillColor.g / 255.0f,
				fillColor.b / 255.0f,
				fillColor.a / 255.0f);

			// Draw polygon mesh dengan offset
			ofMesh polygonMesh;
			for (const auto& v : vertices) {
				polygonMesh.addVertex(ofVec3f(v.x + offset.x, v.y + offset.y, 0.0f));
			}
			polygonMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
			polygonMesh.draw();

			globalPolygonShader.end();
		}
		// Cek FillAnimation (Wave Fill)
		else if (auto* fillAnim = dynamic_cast<FillAnimation*>(animation.get())) {
			// Multi-pass rendering dengan per-polygon FBO

			// Hitung bounding box X dan Y (minX/maxX/minY/maxY)
			// Hitung di sini untuk memastikan nilai tersedia sebelum allocate FBO
			float minX = vertices[0].x, maxX = vertices[0].x;
			float minY = vertices[0].y, maxY = vertices[0].y;
			for (const auto& v : vertices) {
				if (v.x < minX) minX = v.x;
				if (v.x > maxX) maxX = v.x;
				if (v.y < minY) minY = v.y;
				if (v.y > maxY) maxY = v.y;
			}
			float width = maxX - minX;
			float height = maxY - minY;

			// Update member variables untuk currentWaterY calculation
			this->minY = minY;
			this->maxY = maxY;

			// Recalculate currentWaterY dengan minY/maxY yang baru
			float waterLevel = fillAnim->getWaterLevel();
			currentWaterY = maxY - (maxY - minY) * waterLevel;

			// GLOBAL shader (shared untuk semua polygon)
			static ofShader globalFillShader;
			static bool globalShaderLoaded = false;

			// Load shader sekali saja
			if (!globalShaderLoaded) {
				globalFillShader.load("shaders/fillShader.vert", "shaders/waveFill.frag");
				globalShaderLoaded = true;
			}

			// Allocate FBO untuk polygon ini (instance member)
			// Round ke integer untuk menghindari reallocation karena floating point precision
			int fboWidth = (int)std::round(width);
			int fboHeight = (int)std::round(height);

			if (!fboAllocated || lastFboWidth != fboWidth || lastFboHeight != fboHeight) {
				maskFbo.allocate(fboWidth, fboHeight);
				fboAllocated = true;
				lastFboWidth = fboWidth;
				lastFboHeight = fboHeight;
			}

			// Pass 1: Render polygon mask ke FBO (offset ke 0,0)
			maskFbo.begin();
			ofClear(0, 0, 0, 0);
			ofFill();
			ofSetColor(255);
			ofPushMatrix();
			ofTranslate(-minX, -minY);  // Offset supaya polygon mulai dari (0,0)
			ofBeginShape();
			for (auto& v : vertices) {
				ofVertex(v.x, v.y);
			}
			ofEndShape(true);
			ofPopMatrix();
			maskFbo.end();

			// Pass 2: Draw quad dengan wave effect shader
			ofMesh quad;
			quad.addVertex(ofVec3f(minX, minY, 0));
			quad.addVertex(ofVec3f(maxX, minY, 0));
			quad.addVertex(ofVec3f(maxX, maxY, 0));
			quad.addVertex(ofVec3f(minX, maxY, 0));

			quad.addTexCoord(ofVec2f(0, 0));
			quad.addTexCoord(ofVec2f(1, 0));
			quad.addTexCoord(ofVec2f(1, 1));
			quad.addTexCoord(ofVec2f(0, 1));

			quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

			// Draw quad dengan shader
			globalFillShader.begin();
			globalFillShader.setUniformTexture("maskTexture", maskFbo.getTexture(), 0);
			globalFillShader.setUniform4f("fillColor",
				fillColor.r / 255.0f,
				fillColor.g / 255.0f,
				fillColor.b / 255.0f,
				fillColor.a / 255.0f);
			globalFillShader.setUniform1f("minY", minY);
			globalFillShader.setUniform1f("maxY", maxY);
			globalFillShader.setUniform1f("currentWaterY", currentWaterY);
			globalFillShader.setUniform1f("waveAmplitude", fillAnim->getWaveAmplitude());
			globalFillShader.setUniform1f("waveFrequency", fillAnim->getWaveFrequency());
			globalFillShader.setUniform1f("progress", fillAnim->getProgress());
			globalFillShader.setUniform1f("aaMargin", 4.0f);
			globalFillShader.setUniform2f("uMaskSize", maskFbo.getWidth(), maskFbo.getHeight());
			globalFillShader.setUniform2f("uBoundingBoxMin", minX, minY);
			globalFillShader.setUniform2f("uBoundingBoxMax", maxX, maxY);

			quad.draw();

			globalFillShader.end();
		}
		// Unknown animation type - fallback ke no animation
		else {
			// Render dengan shader
			globalPolygonShader.begin();
			globalPolygonShader.setUniform4f("color",
				fillColor.r / 255.0f,
				fillColor.g / 255.0f,
				fillColor.b / 255.0f,
				fillColor.a / 255.0f);

			// Draw polygon mesh
			ofMesh polygonMesh;
			for (const auto& v : vertices) {
				polygonMesh.addVertex(ofVec3f(v.x, v.y, 0.0f));
			}
			polygonMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
			polygonMesh.draw();

			globalPolygonShader.end();
		}
	} else {
		// Tidak ada animation, pakai fillColor asli
		// Render dengan shader
		globalPolygonShader.begin();
		globalPolygonShader.setUniform4f("color",
			fillColor.r / 255.0f,
			fillColor.g / 255.0f,
			fillColor.b / 255.0f,
			fillColor.a / 255.0f);

		// Draw polygon mesh
		ofMesh polygonMesh;
		for (const auto& v : vertices) {
			polygonMesh.addVertex(ofVec3f(v.x, v.y, 0.0f));
		}
		polygonMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		polygonMesh.draw();

		globalPolygonShader.end();
	}

	ofPopStyle();
}

//--------------------------------------------------------------
void PolygonShape::update(float deltaTime) {
	// Update bounding box Y (selalu, untuk FillAnimation)
	if (!vertices.empty()) {
		minY = vertices[0].y;
		maxY = vertices[0].y;
		for (const auto& v : vertices) {
			if (v.y < minY) minY = v.y;
			if (v.y > maxY) maxY = v.y;
		}
	}

	// Update animation jika ada
	if (animation) {
		animation->update(deltaTime);

		// Jika FillAnimation, hitung currentWaterY
		if (auto* fillAnim = dynamic_cast<FillAnimation*>(animation.get())) {
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
