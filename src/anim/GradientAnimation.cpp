#include "GradientAnimation.h"

//--------------------------------------------------------------
GradientAnimation::GradientAnimation(float speed, float frequency)
    : speed(speed), frequency(frequency), AbstractAnimation(1.0f) {
    // Gradient animation selalu complete (no progress tracking)
    progress = 1.0f;
}

//--------------------------------------------------------------
void GradientAnimation::update(float deltaTime) {
    // Gradient animation tidak punya progress
    // Effect di-handle oleh shader menggunakan ofGetElapsedTimef()
}

//--------------------------------------------------------------
float GradientAnimation::getSpeed() const {
    return speed;
}

//--------------------------------------------------------------
void GradientAnimation::setSpeed(float speed) {
    this->speed = speed;
}

//--------------------------------------------------------------
float GradientAnimation::getFrequency() const {
    return frequency;
}

//--------------------------------------------------------------
void GradientAnimation::setFrequency(float frequency) {
    this->frequency = frequency;
}

//--------------------------------------------------------------
void GradientAnimation::reset() {
    // Nothing to reset for gradient animation
}
