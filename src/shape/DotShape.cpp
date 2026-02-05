#include "DotShape.h"
#include "DotInfo.h"
#include <ofMain.h>

DotShape::DotShape(glm::vec2 position, std::string label, float radius)
    : position(position), label(label), radius(radius) {
    // Set maxProgress ke 1 untuk animasi simpel
    maxProgress = 1;
}

//--------------------------------------------------------------
void DotShape::update() {
    if (showing) {
        // Animasi muncul
        if (progress < maxProgress) {
            progress += speed;
            if (progress > maxProgress) {
                progress = maxProgress;
            }
        }
    } else {
        // Animasi hilang (reverse)
        if (progress > 0) {
            progress -= speed;
            if (progress < 0) {
                progress = 0;
            }
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
