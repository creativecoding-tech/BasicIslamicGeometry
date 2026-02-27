#include "DotShape.h"
#include "DotInfo.h"
#include <ofMain.h>

DotShape::DotShape(glm::vec2 position, std::string label, float radius)
    : originalPosition(position), originalLowerBound(position),
      baseRadius(1.0f), position(position), label(label), radius(radius) {
  // Set maxProgress ke 1 untuk animasi simpel
  maxProgress = 1;
}

//--------------------------------------------------------------
void DotShape::update(float deltaTime) {
    // Animasi muncul dari 0 ke maxProgress
    if (progress < maxProgress) {
        progress += speed * deltaTime;
        if (progress > maxProgress) {
            progress = maxProgress;
        }
    }
}

//--------------------------------------------------------------
void DotShape::draw() {
    // Gambar dot dengan animasi fade in/scale
    if (progress <= 0) return;

    ofPushMatrix();
    ofTranslate(position.x, position.y);

    ofFill();
    ofSetColor(color);  // Gunakan warna dari member variable

    // Animasi scale dot
    float scale = progress;  // Scale dari 0 ke 1
    float drawnRadius = radius * scale;  // Gunakan radius member

    ofDrawCircle(0, 0, drawnRadius);

    ofPopMatrix();
}

//--------------------------------------------------------------
void DotShape::addDotsToCache(std::vector<DotInfo>& dots) {
    // Tambahkan dot ini ke cache untuk hover detection
    dots.push_back({position, "Dot"});
}

//--------------------------------------------------------------
void DotShape::saveOriginalPosition(float currentTemplateRadius) {
  originalPosition = position;
  originalLowerBound = lowerBound;
  baseRadius = currentTemplateRadius;
}

//--------------------------------------------------------------
void DotShape::scaleToRadius(float newRadius) {
  if (baseRadius <= 0.0f)
    return;
  float scaleRatio = newRadius / baseRadius;

  if (std::abs(scaleRatio - 1.0f) < 0.0001f) {
    // Tidak ada scaling yang signifikan, restore ke original
    position = originalPosition;
    lowerBound = originalLowerBound;
  } else {
    // Scale position dan lowerBound
    position = originalPosition * scaleRatio;
    lowerBound = originalLowerBound * scaleRatio;
  }
}
