#include "CircleShape.h"
#include "DotInfo.h"
#include <ofMain.h>

CircleShape::CircleShape(float r, std::string label, float angle,
                         float distance) {
  this->radius = r;
  this->originalRadius =
      r; // Simpan radius awal untuk proportional recalculation
  this->label = label;
  this->angle = angle;
  this->distance = distance;
  this->originalDistance = distance;

  loadFonts();                 // Load font dari AbstractShape
  maxProgress = totalSegments; // Set max progress untuk isComplete()
}

void CircleShape::setLabel(std::string label) { this->label = label; }

void CircleShape::setRadius(float r) {
  if (originalRadius <= 0.0f)
    return;
  // Re-calculate distance secara proporsional dari original
  float scaleFactor = r / originalRadius;

  if (std::abs(scaleFactor - 1.0f) < 0.0001f) {
    distance = originalDistance;
  } else {
    distance = originalDistance * scaleFactor;
  }

  radius = r;
  // DO NOT update originalRadius or geometric originals to prevent drift
}

void CircleShape::showLabel() { labelVisible = true; }

void CircleShape::hideLabel() { labelVisible = false; }

void CircleShape::showDot() { dotVisible = true; }

void CircleShape::hideDot() { dotVisible = false; }

void CircleShape::update(float deltaTime) {
  // Animasi muncul dari 0 ke totalSegments
  if (progress < totalSegments)
    progress += speed * deltaTime;
}

void CircleShape::draw() {
  ofPushMatrix();

  // Hitung pos dari angle dan distance
  float x = cos(angle) * distance;
  float y = sin(angle) * distance;
  ofTranslate(x, y);

  ofNoFill();
  ofSetColor(0);
  ofSetLineWidth(lineWidth);

  ofPolyline polyline;

  // Hitung berapa banyak segment yang harus digambar
  int segmentsToDraw = (int)progress;

  // Gambar arc - dari angle 0 sampai circle progress
  for (int i = 0; i <= segmentsToDraw; i++) {
    // Map dari 0-100 ke 0-TWO_PI, tapi tambahkan sedikit buffer
    float drawAngle = ofMap(i, 0, totalSegments, 0, TWO_PI);

    // Kalau ini adalah segment terakhir dan kita sudah full, pastikan angle = 0
    // (tutup loop)
    if (i == totalSegments && progress >= totalSegments) {
      drawAngle = 0; // Kembali ke titik awal
    }

    float px = cos(drawAngle) * radius;
    float py = sin(drawAngle) * radius;
    polyline.addVertex(px, py);
  }
  polyline.close();
  polyline.draw();

  if (progress >= totalSegments) {
    ofFill();
    // Gambar dot hanya jika dotVisible = true
    if (dotVisible) {
      ofDrawCircle(0, 0, lineWidth * 2);
    }
    ofSetColor(0);
    if (labelVisible) {
      fontNormal.drawString(label, 10, -5);
    }
  }
  ofPopMatrix();
}

//--------------------------------------------------------------
void CircleShape::addDotsToCache(std::vector<DotInfo> &dots) {
  // Hitung pos dari angle dan distance
  float x = cos(angle) * distance;
  float y = sin(angle) * distance;
  dots.push_back({glm::vec2(x, y), "Circle"});
}
