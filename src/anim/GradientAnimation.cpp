#include "GradientAnimation.h"

//--------------------------------------------------------------
GradientAnimation::GradientAnimation(float speed, float frequency, float duration)
    : speed(speed), frequency(frequency), duration(duration), time(0.0f), AbstractAnimation(duration) {
    // Gradient animation finite: berjalan selama duration detik
    progress = 0.0f;
}

//--------------------------------------------------------------
void GradientAnimation::update(float deltaTime) {
    // Accumulate time
    time += deltaTime;

    // Update progress berdasarkan duration
    progress = time / duration;
    if (progress > 1.0f) {
        progress = 1.0f;
    }
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
float GradientAnimation::getDuration() const {
    return duration;
}

//--------------------------------------------------------------
void GradientAnimation::setDuration(float duration) {
    this->duration = duration;
}

//--------------------------------------------------------------
float GradientAnimation::getTime() const {
    return time;
}

//--------------------------------------------------------------
void GradientAnimation::reset() {
    time = 0.0f;
    progress = 0.0f;
}
