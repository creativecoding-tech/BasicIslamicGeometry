#include "WobbleFillAnimation.h"

//--------------------------------------------------------------
WobbleFillAnimation::WobbleFillAnimation(float wobbleAmount, float wobbleSpeed,
                                         float wobbleFrequency, float targetFillLevel,
                                         float fillSpeed)
    : wobbleAmount(wobbleAmount), wobbleSpeed(wobbleSpeed),
      wobbleFrequency(wobbleFrequency), targetFillLevel(targetFillLevel),
      currentFillLevel(0.0f), fillSpeed(fillSpeed),
      time(0.0f), AbstractAnimation(10.0f) {
    // WobbleFill animation berjalan terus menerus
    // Fill bertambah perlahan dari 0 ke targetFillLevel (default 100%)
    // Kalau mau 50%, panggil dengan targetFillLevel=0.5
}

//--------------------------------------------------------------
void WobbleFillAnimation::update(float deltaTime) {
    // Update time untuk wobble calculation
    time += deltaTime * wobbleSpeed;

    // Gradual fill: bertambah perlahan ke target
    if (currentFillLevel < targetFillLevel) {
        currentFillLevel += deltaTime * fillSpeed;
        if (currentFillLevel > targetFillLevel) {
            currentFillLevel = targetFillLevel;
        }
    }

    // Animation complete ketika fill sudah mencapai target
    progress = currentFillLevel / targetFillLevel;
}

//--------------------------------------------------------------
glm::vec2 WobbleFillAnimation::getWobbleOffset() const {
    // Calculate wobble offset menggunakan sin/cos
    // Sama seperti WOBBLE animation
    float offsetX = cos(time * wobbleFrequency) * wobbleAmount;
    float offsetY = sin(time * wobbleFrequency * 0.7f) * wobbleAmount;
    return glm::vec2(offsetX, offsetY);
}

//--------------------------------------------------------------
float WobbleFillAnimation::getFillLevel() const {
    return currentFillLevel;
}

//--------------------------------------------------------------
void WobbleFillAnimation::setWobbleAmount(float amount) {
    wobbleAmount = amount;
}

//--------------------------------------------------------------
float WobbleFillAnimation::getWobbleAmount() const {
    return wobbleAmount;
}

//--------------------------------------------------------------
void WobbleFillAnimation::setWobbleSpeed(float speed) {
    wobbleSpeed = speed;
}

//--------------------------------------------------------------
float WobbleFillAnimation::getWobbleSpeed() const {
    return wobbleSpeed;
}

//--------------------------------------------------------------
void WobbleFillAnimation::setWobbleFrequency(float freq) {
    wobbleFrequency = freq;
}

//--------------------------------------------------------------
float WobbleFillAnimation::getWobbleFrequency() const {
    return wobbleFrequency;
}

//--------------------------------------------------------------
void WobbleFillAnimation::setTargetFillLevel(float level) {
    targetFillLevel = glm::clamp(level, 0.0f, 1.0f);
}

//--------------------------------------------------------------
float WobbleFillAnimation::getTargetFillLevel() const {
    return targetFillLevel;
}

//--------------------------------------------------------------
void WobbleFillAnimation::setFillSpeed(float speed) {
    fillSpeed = speed;
}

//--------------------------------------------------------------
float WobbleFillAnimation::getFillSpeed() const {
    return fillSpeed;
}

//--------------------------------------------------------------
void WobbleFillAnimation::reset() {
    time = 0.0f;
    currentFillLevel = 0.0f;  // Reset fill ke 0
    progress = 0.0f;
}
