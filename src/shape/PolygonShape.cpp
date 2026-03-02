#include "PolygonShape.h"
#include "../anim/FadeInAnimation.h"
#include "../anim/FillAnimation.h"
#include "../anim/GradientAnimation.h"
#include "../anim/RotateLeftAnimation.h"
#include "../anim/RotateRightAnimation.h"
#include "../anim/SpinLeftAnimation.h"
#include "../anim/SpinRightAnimation.h"
#include "../anim/WobbleAnimation.h"
#include "../anim/WobbleFillAnimation.h"

//--------------------------------------------------------------
PolygonShape::PolygonShape()
    : vertices(), originalVertices(), baseRadius(1.0f),
      fillColor(ofColor(255, 0, 0, 150)), selected(false), index(-1),
      loadedFromFile(false), tessellated(false), sourceTessellationFile(""),
      sourceTessellationRadius(10.0f), animation(nullptr),
      specialAnimation(nullptr), minX(0.0f), maxX(0.0f), minY(0.0f), maxY(0.0f),
      currentWaterY(0.0f), shaderLoaded(false), fboAllocated(false),
      lastFboWidth(0), lastFboHeight(0) {
  updateBounds();
}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color)
    : vertices(verts), originalVertices(verts), baseRadius(1.0f),
      fillColor(color), selected(false), index(-1), loadedFromFile(false),
      tessellated(false), sourceTessellationFile(""),
      sourceTessellationRadius(10.0f), animation(nullptr),
      specialAnimation(nullptr), minX(0.0f), maxX(0.0f), minY(0.0f), maxY(0.0f),
      currentWaterY(0.0f), shaderLoaded(false), fboAllocated(false),
      lastFboWidth(0), lastFboHeight(0) {
  updateBounds();
}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color, int idx)
    : vertices(verts), originalVertices(verts), baseRadius(1.0f),
      fillColor(color), selected(false), index(idx), loadedFromFile(false),
      tessellated(false), sourceTessellationFile(""),
      sourceTessellationRadius(10.0f), animation(nullptr),
      specialAnimation(nullptr), minX(0.0f), maxX(0.0f), minY(0.0f), maxY(0.0f),
      currentWaterY(0.0f), shaderLoaded(false), fboAllocated(false),
      lastFboWidth(0), lastFboHeight(0) {
  updateBounds();
}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color, int index,
                           std::shared_ptr<AbstractAnimation> anim)
    : vertices(verts), originalVertices(verts), baseRadius(1.0f),
      fillColor(color), selected(false), index(index), loadedFromFile(false),
      tessellated(false), sourceTessellationFile(""),
      sourceTessellationRadius(10.0f), animation(std::move(anim)),
      specialAnimation(nullptr), minX(0.0f), maxX(0.0f), minY(0.0f), maxY(0.0f),
      currentWaterY(0.0f), shaderLoaded(false), fboAllocated(false),
      lastFboWidth(0), lastFboHeight(0) {
  updateBounds();
}

//--------------------------------------------------------------
PolygonShape::PolygonShape(vector<vec2> verts, ofColor color, int index,
                           std::shared_ptr<AbstractAnimation> anim,
                           std::shared_ptr<AbstractAnimation> specialAnim)
    : vertices(verts), originalVertices(verts), baseRadius(1.0f),
      fillColor(color), selected(false), index(index), loadedFromFile(false),
      tessellated(false), sourceTessellationFile(""),
      sourceTessellationRadius(10.0f), animation(std::move(anim)),
      specialAnimation(std::move(specialAnim)), minX(0.0f), maxX(0.0f),
      minY(0.0f), maxY(0.0f), currentWaterY(0.0f), shaderLoaded(false),
      fboAllocated(false), lastFboWidth(0), lastFboHeight(0) {
  updateBounds();
}

//--------------------------------------------------------------
PolygonShape::PolygonShape(const PolygonShape &other)
    : vertices(other.vertices), originalVertices(other.originalVertices),
      baseRadius(other.baseRadius), fillColor(other.fillColor),
      selected(other.selected), index(other.index),
      loadedFromFile(other.loadedFromFile), tessellated(other.tessellated),
      sourceTessellationFile(other.sourceTessellationFile),
      sourceTessellationRadius(other.sourceTessellationRadius),
      animation(nullptr), specialAnimation(nullptr), minX(0.0f), maxX(0.0f),
      minY(0.0f), maxY(0.0f), currentWaterY(0.0f), shaderLoaded(false),
      fboAllocated(false), lastFboWidth(0), lastFboHeight(0) {
  updateBounds();
}

//--------------------------------------------------------------
PolygonShape &PolygonShape::operator=(const PolygonShape &other) {
  if (this != &other) {
    vertices = other.vertices;
    originalVertices = other.originalVertices;
    baseRadius = other.baseRadius;
    fillColor = other.fillColor;
    selected = other.selected;
    index = other.index;
    loadedFromFile = other.loadedFromFile;
    tessellated = other.tessellated;
    sourceTessellationFile = other.sourceTessellationFile;
    sourceTessellationRadius = other.sourceTessellationRadius;
    animation = nullptr; // Animation tidak dicopy (reset ke nullptr)
    specialAnimation = nullptr;
    updateBounds();
  }
  return *this;
}

//--------------------------------------------------------------
// Move constructor - transfer animation ownership
PolygonShape::PolygonShape(PolygonShape &&other) noexcept
    : vertices(std::move(other.vertices)),
      originalVertices(std::move(other.originalVertices)),
      baseRadius(other.baseRadius), fillColor(other.fillColor),
      selected(other.selected), index(other.index),
      loadedFromFile(other.loadedFromFile), tessellated(other.tessellated),
      sourceTessellationFile(std::move(other.sourceTessellationFile)),
      sourceTessellationRadius(other.sourceTessellationRadius),
      animation(std::move(other.animation)),
      specialAnimation(std::move(other.specialAnimation)), minX(0.0f),
      maxX(0.0f), minY(0.0f), maxY(0.0f), currentWaterY(0.0f),
      shaderLoaded(false), fboAllocated(false), lastFboWidth(0),
      lastFboHeight(0) {
  updateBounds();
}

//--------------------------------------------------------------
// Move assignment operator - transfer animation ownership
PolygonShape &PolygonShape::operator=(PolygonShape &&other) noexcept {
  if (this != &other) {
    vertices = std::move(other.vertices);
    originalVertices = std::move(other.originalVertices);
    baseRadius = other.baseRadius;
    fillColor = other.fillColor;
    selected = other.selected;
    index = other.index;
    loadedFromFile = other.loadedFromFile;
    tessellated = other.tessellated;
    sourceTessellationFile = std::move(other.sourceTessellationFile);
    sourceTessellationRadius = other.sourceTessellationRadius;
    animation = std::move(other.animation);
    specialAnimation = std::move(other.specialAnimation);
    updateBounds();
  }
  return *this;
}

//--------------------------------------------------------------
void PolygonShape::saveOriginalVertices(float currentTemplateRadius) {
  originalVertices = vertices;
  baseRadius = currentTemplateRadius;
}

//--------------------------------------------------------------
void PolygonShape::scaleToRadius(float newRadius) {
  if (baseRadius <= 0.0f || originalVertices.empty())
    return;
  float scaleRatio = newRadius / baseRadius;

  if (std::abs(scaleRatio - 1.0f) < 0.0001f) {
    vertices = originalVertices;
  } else {
    vertices.resize(originalVertices.size());
    for (size_t i = 0; i < originalVertices.size(); ++i) {
      vertices[i] = originalVertices[i] * scaleRatio;
    }
  }
  updateBounds();
}

//--------------------------------------------------------------
void PolygonShape::draw() const {
  ofPushStyle();
  ofFill();

  // Conditional rendering berdasarkan asal polygon
  if (loadedFromFile) {
    // Polygon diload dari file .nay → Gunakan GLSL shaders
    drawGLSL();
  } else {
    // Polygon dibuat baru (CTRL+G / right-click) → Gunakan CPU rendering
    drawCPU();
  }

  ofPopStyle();
}

//--------------------------------------------------------------
void PolygonShape::update(float deltaTime) {
  // Update bounding box X and Y (selalu, untuk FillAnimation dan AABB check)
  updateBounds();

  // Update animation jika ada
  if (animation) {
    animation->update(deltaTime);

    // Jika FillAnimation, hitung currentWaterY
    if (auto *fillAnim = dynamic_cast<FillAnimation *>(animation.get())) {
      // Hitung current water Y (naik dari bawah ke atas)
      float waterLevel = fillAnim->getWaterLevel();
      currentWaterY = maxY - (maxY - minY) * waterLevel;
    }

    // Jika FillAnimation complete, hapus animation dan switch ke No Animation
    if (auto *fillAnim = dynamic_cast<FillAnimation *>(animation.get())) {
      if (fillAnim->isComplete()) {
        animation = nullptr; // Switch ke No Animation
      }
    }

    // Jika WobbleFillAnimation complete, hapus animation dan switch ke No
    // Animation
    if (auto *wobbleFillAnim =
            dynamic_cast<WobbleFillAnimation *>(animation.get())) {
      if (wobbleFillAnim->isComplete()) {
        animation = nullptr; // Switch ke No Animation
      }
    }

    // Jika GradientAnimation complete, hapus animation dan switch ke No
    // Animation
    if (auto *gradientAnim =
            dynamic_cast<GradientAnimation *>(animation.get())) {
      if (gradientAnim->isComplete()) {
        animation = nullptr; // Switch ke No Animation
      }
    }
  }

  // Update special animation only after appearance animation is complete (atau
  // tidak ada)
  if (isAnimationComplete() && specialAnimation) {
    specialAnimation->update(deltaTime);
  }
}

//--------------------------------------------------------------
bool PolygonShape::hasAnimation() const { return animation != nullptr; }

//--------------------------------------------------------------
bool PolygonShape::isAnimationComplete() const {
  if (animation) {
    return animation->isComplete();
  }
  return true; // Kalau tidak ada animation, dianggap complete
}

//--------------------------------------------------------------
bool PolygonShape::isSpecialAnimationComplete() const {
  if (specialAnimation) {
    return specialAnimation->isComplete();
  }
  return true;
}

//--------------------------------------------------------------
void PolygonShape::setColor(ofColor color) { fillColor = color; }

//--------------------------------------------------------------
void PolygonShape::setSelected(bool sel) { selected = sel; }

//--------------------------------------------------------------
void PolygonShape::setVertices(const vector<vec2> &verts) {
  vertices = verts;
  updateBounds();
}

//--------------------------------------------------------------
void PolygonShape::setSpeed(float speed) {
  if (animation) {
    animation->setSpeed(speed);
  }
  if (specialAnimation) {
    specialAnimation->setSpeed(speed);
  }
}

//--------------------------------------------------------------
void PolygonShape::setSpeedMultiplier(float multiplier) {
  if (animation) {
    animation->setSpeedMultiplier(multiplier); // Delegate to appearance animation only
  }
  // Special animation punya speed multiplier sendiri, jangan di-override di sini
}

//--------------------------------------------------------------
ofColor PolygonShape::getColor() const { return fillColor; }

//--------------------------------------------------------------
bool PolygonShape::isSelected() const { return selected; }

//--------------------------------------------------------------
const vector<vec2> &PolygonShape::getVertices() const { return vertices; }

//--------------------------------------------------------------
bool PolygonShape::containsPoint(vec2 point) const {
  if (vertices.size() < 3)
    return false;

  // 1. Early-exit AABB check
  // Pastikan update() sudah dipanggil perlahan setelah titik divariasikan agar
  // minX/maxX up-to-date
  if (point.x < minX || point.x > maxX || point.y < minY || point.y > maxY) {
    return false;
  }

  // 2. Ray-casting algorithm (Sangat ringan dibanding membuat ofPolyline baru)
  bool inside = false;
  for (size_t i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
    if (((vertices[i].y > point.y) != (vertices[j].y > point.y)) &&
        (point.x < (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) /
                           (vertices[j].y - vertices[i].y) +
                       vertices[i].x)) {
      inside = !inside;
    }
  }

  return inside;
}

//--------------------------------------------------------------
void PolygonShape::setAnimation(std::shared_ptr<AbstractAnimation> anim) {
  animation = std::move(anim);
}

//--------------------------------------------------------------
std::shared_ptr<AbstractAnimation> PolygonShape::getAnimationPtr() const {
  return animation;
}

//--------------------------------------------------------------
void PolygonShape::setSpecialAnimation(
    std::shared_ptr<AbstractAnimation> anim) {
  specialAnimation = std::move(anim);
}

//--------------------------------------------------------------
std::shared_ptr<AbstractAnimation>
PolygonShape::getSpecialAnimationPtr() const {
  return specialAnimation;
}

//--------------------------------------------------------------
void PolygonShape::updateBounds() {
  if (vertices.empty()) {
    minX = maxX = minY = maxY = 0.0f;
    return;
  }
  minX = maxX = vertices[0].x;
  minY = maxY = vertices[0].y;
  for (const auto &v : vertices) {
    if (v.x < minX)
      minX = v.x;
    if (v.x > maxX)
      maxX = v.x;
    if (v.y < minY)
      minY = v.y;
    if (v.y > maxY)
      maxY = v.y;
  }
}

//--------------------------------------------------------------
// CPU-based rendering untuk newly created polygons (CTRL+G / right-click)
// Polygon baru TIDAK punya animasi, jadi simple rendering saja
void PolygonShape::drawCPU() const {
  ofSetColor(fillColor);

  ofBeginShape();
  for (auto &v : vertices) {
    ofVertex(v.x, v.y);
  }
  ofEndShape(true);
}

//--------------------------------------------------------------
// GPU-based rendering untuk polygons loaded dari .nay files
void PolygonShape::drawGLSL() const {
  // GLOBAL shader untuk basic polygon rendering (semua polygon kecuali Wave
  // Fill)
  static ofShader globalPolygonShader;
  static bool globalPolygonShaderLoaded = false;

  // Load shader sekali saja
  if (!globalPolygonShaderLoaded) {
    globalPolygonShader.load("shaders/polygon.vert", "shaders/polygon.frag");
    globalPolygonShaderLoaded = true;
  }

  // Cek Special Animation yang berlaku global ke seluruh shape (seperti
  // rotation)
  bool hasRotation = false;
  bool hasSpin = false;

  if (specialAnimation) {
    if (auto *rotateAnim =
            dynamic_cast<RotateLeftAnimation *>(specialAnimation.get())) {
      ofPushMatrix();
      ofRotateDeg(rotateAnim->getAngle());
      hasRotation = true;
    } else if (auto *rotateRightAnim =
                   dynamic_cast<RotateRightAnimation *>(specialAnimation.get())) {
      ofPushMatrix();
      ofRotateDeg(rotateRightAnim->getAngle());
      hasRotation = true;
    } else if (auto *spinLeftAnim =
                   dynamic_cast<SpinLeftAnimation *>(specialAnimation.get())) {
      // SPIN ANIMATION: Berputar pada porosnya sendiri (centroid)
      // Hitung centroid
      vec2 centroid(0.0f, 0.0f);
      for (const auto &v : vertices) {
        centroid += v;
      }
      centroid /= vertices.size();

      // Apply transform: Translate -> Rotate -> Translate back
      ofPushMatrix();
      ofTranslate(centroid.x, centroid.y);
      ofRotateDeg(spinLeftAnim->getAngle());
      ofTranslate(-centroid.x, -centroid.y);
      hasSpin = true;
    } else if (auto *spinRightAnim =
                   dynamic_cast<SpinRightAnimation *>(specialAnimation.get())) {
      // SPIN ANIMATION: Berputar pada porosnya sendiri (centroid)
      // Hitung centroid
      vec2 centroid(0.0f, 0.0f);
      for (const auto &v : vertices) {
        centroid += v;
      }
      centroid /= vertices.size();

      // Apply transform: Translate -> Rotate -> Translate back
      ofPushMatrix();
      ofTranslate(centroid.x, centroid.y);
      ofRotateDeg(spinRightAnim->getAngle());
      ofTranslate(-centroid.x, -centroid.y);
      hasSpin = true;
    }
  }

  // Cek tipe animation dan apply effect yang sesuai
  if (animation) {
    ofColor animatedColor = fillColor;

    // Cek FadeInAnimation
    if (auto *fadeAnim = dynamic_cast<FadeInAnimation *>(animation.get())) {
      float progress = animation->getProgress();

      // Kalau progress masih 0, alpha harus 0 (BENAR-BENAR HIDDEN)
      if (progress == 0.0f) {
        animatedColor.a = 0; // FORCE alpha 0!
      } else {
        // Progress > 0, pakai current alpha
        float currentAlpha = fadeAnim->getCurrentAlpha();
        animatedColor.a = static_cast<int>(currentAlpha);
      }

      // Render dengan shader + ofBeginShape (untuk support tessellated curve)
      globalPolygonShader.begin();
      globalPolygonShader.setUniform4f(
          "color", animatedColor.r / 255.0f, animatedColor.g / 255.0f,
          animatedColor.b / 255.0f, animatedColor.a / 255.0f);

      // Draw polygon dengan ofBeginShape (bukan ofMesh) untuk tessellated curve
      ofBeginShape();
      for (const auto &v : vertices) {
        ofVertex(v.x, v.y);
      }
      ofEndShape(true);

      globalPolygonShader.end();
    }
    // Cek WobbleAnimation (Wobble biasa, tanpa fill)
    else if (auto *wobbleAnim =
                 dynamic_cast<WobbleAnimation *>(animation.get())) {
      // Ambil wobble offset untuk vertex manipulation
      glm::vec2 wobbleOffset = wobbleAnim->getWobbleOffset();

      // GLOBAL shader untuk wobble (shared untuk semua polygon)
      static ofShader globalWobbleShader;
      static bool globalWobbleShaderLoaded = false;

      // Load shader sekali saja
      if (!globalWobbleShaderLoaded) {
        globalWobbleShader.load("shaders/wobble.vert", "shaders/wobble.frag");
        globalWobbleShaderLoaded = true;
      }

      // Render dengan shader wobble
      globalWobbleShader.begin();
      globalWobbleShader.setUniform4f(
          "fillColor", fillColor.r / 255.0f, fillColor.g / 255.0f,
          fillColor.b / 255.0f, fillColor.a / 255.0f);
      globalWobbleShader.setUniform2f("uWobbleOffset", wobbleOffset.x,
                                      wobbleOffset.y);
      globalWobbleShader.setUniform1f("uWobbleAmount",
                                      1.0f); // Full wobble effect

      // Draw polygon dengan ofBeginShape (untuk support tessellated curve)
      ofBeginShape();
      for (const auto &v : vertices) {
        ofVertex(v.x, v.y);
      }
      ofEndShape(true);

      globalWobbleShader.end();
    }
    // Cek FillAnimation (Wave Fill)
    else if (auto *fillAnim = dynamic_cast<FillAnimation *>(animation.get())) {
      // Multi-pass rendering dengan per-polygon FBO

      // Hitung bounding box X dan Y (minX/maxX/minY/maxY)
      float minX = vertices[0].x, maxX = vertices[0].x;
      float minY = vertices[0].y, maxY = vertices[0].y;
      for (const auto &v : vertices) {
        if (v.x < minX)
          minX = v.x;
        if (v.x > maxX)
          maxX = v.x;
        if (v.y < minY)
          minY = v.y;
        if (v.y > maxY)
          maxY = v.y;
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
        globalFillShader.load("shaders/fillShader.vert",
                              "shaders/waveFill.frag");
        globalShaderLoaded = true;
      }

      // Allocate FBO untuk polygon ini (instance member)
      int fboWidth = (int)std::round(width);
      int fboHeight = (int)std::round(height);

      if (!fboAllocated || lastFboWidth != fboWidth ||
          lastFboHeight != fboHeight) {
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
      ofTranslate(-minX, -minY);
      ofBeginShape();
      for (auto &v : vertices) {
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
      globalFillShader.setUniformTexture("maskTexture", maskFbo.getTexture(),
                                         0);
      globalFillShader.setUniform4f("fillColor", fillColor.r / 255.0f,
                                    fillColor.g / 255.0f, fillColor.b / 255.0f,
                                    fillColor.a / 255.0f);
      globalFillShader.setUniform1f("minY", minY);
      globalFillShader.setUniform1f("maxY", maxY);
      globalFillShader.setUniform1f("currentWaterY", currentWaterY);
      globalFillShader.setUniform1f("waveAmplitude",
                                    fillAnim->getWaveAmplitude());
      globalFillShader.setUniform1f("waveFrequency",
                                    fillAnim->getWaveFrequency());
      globalFillShader.setUniform1f("progress", fillAnim->getProgress());
      globalFillShader.setUniform1f("aaMargin", 4.0f);
      globalFillShader.setUniform2f("uMaskSize", maskFbo.getWidth(),
                                    maskFbo.getHeight());
      globalFillShader.setUniform2f("uBoundingBoxMin", minX, minY);
      globalFillShader.setUniform2f("uBoundingBoxMax", maxX, maxY);

      quad.draw();

      globalFillShader.end();
    }
    // Cek WobbleFillAnimation (Wobble Fill - polygon bergerak-goyang + gradual
    // fill)
    else if (auto *wobbleFillAnim =
                 dynamic_cast<WobbleFillAnimation *>(animation.get())) {
      // 2-pass rendering dengan mask + wobble + gradual fill

      // Hitung bounding box
      float minX = vertices[0].x, maxX = vertices[0].x;
      float minY = vertices[0].y, maxY = vertices[0].y;
      for (const auto &v : vertices) {
        if (v.x < minX)
          minX = v.x;
        if (v.x > maxX)
          maxX = v.x;
        if (v.y < minY)
          minY = v.y;
        if (v.y > maxY)
          maxY = v.y;
      }
      float width = maxX - minX;
      float height = maxY - minY;

      // Ambil wobble offset (animation sudah diupdate di method update())
      glm::vec2 wobbleOffset = wobbleFillAnim->getWobbleOffset();

      // GLOBAL shader untuk wobble fill
      static ofShader globalWobbleFillShader;
      static bool globalWobbleShaderLoaded = false;

      if (!globalWobbleShaderLoaded) {
        globalWobbleFillShader.load("shaders/fillShader.vert",
                                    "shaders/wobbleFill.frag");
        globalWobbleShaderLoaded = true;
      }

      // Allocate FBO untuk polygon ini
      int fboWidth = (int)std::round(width);
      int fboHeight = (int)std::round(height);

      if (!fboAllocated || lastFboWidth != fboWidth ||
          lastFboHeight != fboHeight) {
        maskFbo.allocate(fboWidth, fboHeight);
        fboAllocated = true;
        lastFboWidth = fboWidth;
        lastFboHeight = fboHeight;
      }

      // Pass 1: Render polygon STATIS sebagai mask ke FBO (offset ke 0,0)
      maskFbo.begin();
      ofClear(0, 0, 0, 0);
      ofFill();
      ofSetColor(255);
      ofPushMatrix();
      ofTranslate(-minX, -minY);
      ofBeginShape();
      for (auto &v : vertices) {
        ofVertex(v.x, v.y);
      }
      ofEndShape(true);
      ofPopMatrix();
      maskFbo.end();

      // Pass 2: Draw quad dengan shader untuk wobble + gradual fill
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
      globalWobbleFillShader.begin();
      globalWobbleFillShader.setUniformTexture("maskTexture",
                                               maskFbo.getTexture(), 0);
      globalWobbleFillShader.setUniform4f(
          "fillColor", fillColor.r / 255.0f, fillColor.g / 255.0f,
          fillColor.b / 255.0f, fillColor.a / 255.0f);
      globalWobbleFillShader.setUniform1f("fillLevel",
                                          wobbleFillAnim->getFillLevel());
      globalWobbleFillShader.setUniform1f("minY", minY);
      globalWobbleFillShader.setUniform1f("maxY", maxY);
      globalWobbleFillShader.setUniform2f("uMaskSize", maskFbo.getWidth(),
                                          maskFbo.getHeight());
      globalWobbleFillShader.setUniform2f("uWobbleOffset", wobbleOffset.x,
                                          wobbleOffset.y);

      quad.draw();

      globalWobbleFillShader.end();
    }
    // Cek GradientAnimation (Gradient flow - polygon statis, warna gradient
    // yang bergerak)
    else if (auto *gradientAnim =
                 dynamic_cast<GradientAnimation *>(animation.get())) {
      // Multi-pass rendering dengan per-polygon FBO

      // Hitung bounding box X dan Y (minX/maxX/minY/maxY)
      float minX = vertices[0].x, maxX = vertices[0].x;
      float minY = vertices[0].y, maxY = vertices[0].y;
      for (const auto &v : vertices) {
        if (v.x < minX)
          minX = v.x;
        if (v.x > maxX)
          maxX = v.x;
        if (v.y < minY)
          minY = v.y;
        if (v.y > maxY)
          maxY = v.y;
      }
      float width = maxX - minX;
      float height = maxY - minY;

      // GLOBAL shader untuk gradient
      static ofShader globalGradientShader;
      static bool globalGradientShaderLoaded = false;

      // Load shader sekali saja
      if (!globalGradientShaderLoaded) {
        globalGradientShader.load("shaders/gradient.vert",
                                  "shaders/gradient.frag");
        globalGradientShaderLoaded = true;
      }

      // Allocate FBO untuk polygon ini (instance member)
      int fboWidth = (int)std::round(width);
      int fboHeight = (int)std::round(height);

      if (!fboAllocated || lastFboWidth != fboWidth ||
          lastFboHeight != fboHeight) {
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
      ofTranslate(-minX, -minY);
      ofBeginShape();
      for (auto &v : vertices) {
        ofVertex(v.x, v.y);
      }
      ofEndShape(true);
      ofPopMatrix();
      maskFbo.end();

      // Pass 2: Draw quad dengan gradient shader
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
      globalGradientShader.begin();
      globalGradientShader.setUniformTexture("maskTexture",
                                             maskFbo.getTexture(), 0);
      globalGradientShader.setUniform4f(
          "fillColor", fillColor.r / 255.0f, fillColor.g / 255.0f,
          fillColor.b / 255.0f, fillColor.a / 255.0f);
      globalGradientShader.setUniform1f("time", gradientAnim->getTime());
      globalGradientShader.setUniform1f("gradientSpeed",
                                        gradientAnim->getSpeed());
      globalGradientShader.setUniform1f("gradientFrequency",
                                        gradientAnim->getFrequency());
      globalGradientShader.setUniform2f("uMaskSize", maskFbo.getWidth(),
                                        maskFbo.getHeight());
      globalGradientShader.setUniform2f("uBoundingBoxMin", minX, minY);
      globalGradientShader.setUniform2f("uBoundingBoxMax", maxX, maxY);

      quad.draw();

      globalGradientShader.end();
    }
    // Unknown animation type - fallback ke no animation
    else {
      // Render dengan shader + ofBeginShape (untuk support tessellated curve)
      globalPolygonShader.begin();
      globalPolygonShader.setUniform4f(
          "color", fillColor.r / 255.0f, fillColor.g / 255.0f,
          fillColor.b / 255.0f, fillColor.a / 255.0f);

      // Draw polygon dengan ofBeginShape (bukan ofMesh) untuk tessellated curve
      ofBeginShape();
      for (const auto &v : vertices) {
        ofVertex(v.x, v.y);
      }
      ofEndShape(true);

      globalPolygonShader.end();
    }
  } else {
    // Tidak ada animation, pakai fillColor asli
    // Render dengan shader + ofBeginShape (untuk support tessellated curve)
    globalPolygonShader.begin();
    globalPolygonShader.setUniform4f("color", fillColor.r / 255.0f,
                                     fillColor.g / 255.0f, fillColor.b / 255.0f,
                                     fillColor.a / 255.0f);

    // Draw polygon dengan ofBeginShape (bukan ofMesh) untuk tessellated curve
    ofBeginShape();
    for (const auto &v : vertices) {
      ofVertex(v.x, v.y);
    }
    ofEndShape(true);

    globalPolygonShader.end();
  }

  if (hasRotation || hasSpin) {
    ofPopMatrix();
  }
}
