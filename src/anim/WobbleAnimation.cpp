#include "WobbleAnimation.h"
#include <cmath>

//--------------------------------------------------------------
WobbleAnimation::WobbleAnimation(float amplitude, float frequency, float speed)
    : AbstractAnimation(speed), amplitude(amplitude), frequency(frequency) {
}

//--------------------------------------------------------------
void WobbleAnimation::update() {
    if (progress < 1.0f) {
        progress += speed;
        if (progress > 1.0f) {
            progress = 1.0f;
        }
    }
}

//--------------------------------------------------------------
float WobbleAnimation::getWobbleFactor() const {
    // Sinusoidal wobble: -1.0 sampai 1.0
    // Frequency mengontrol berapa banyak siklus sinus
    // Progress 0 → 1.0 mengontrol waktu

    // Decay amplitude seiring progress (wobble berkurang di akhir)
    float decay = 1.0f - progress;  // 1.0 di awal, 0.0 di akhir
    float wobble = std::sin(progress * frequency * PI * 2.0f) * decay;

    return wobble;
}

//--------------------------------------------------------------
glm::vec2 WobbleAnimation::getWobbleOffset() const {
    // Return offset vector untuk vertex manipulation
    float factor = getWobbleFactor();
    return glm::vec2(factor * amplitude, factor * amplitude);
}

//--------------------------------------------------------------
float WobbleAnimation::getWobbleScale() const {
    // Return scale factor untuk radius/size modification
    // 1.0 + (wobble * amplitude_percentage)
    float factor = getWobbleFactor();

    // Amplitude sebagai percentage (misal 20.0 = 20% variation)
    float ampPercentage = amplitude / 100.0f;

    return 1.0f + (factor * ampPercentage);
}

//--------------------------------------------------------------
void WobbleAnimation::setAmplitude(float amplitude) {
    this->amplitude = amplitude;
}

//--------------------------------------------------------------
float WobbleAnimation::getAmplitude() const {
    return amplitude;
}

//--------------------------------------------------------------
void WobbleAnimation::setFrequency(float frequency) {
    this->frequency = frequency;
}

//--------------------------------------------------------------
float WobbleAnimation::getFrequency() const {
    return frequency;
}

//--------------------------------------------------------------
void WobbleAnimation::reset() {
    AbstractAnimation::reset();
    // Tidak ada additional state untuk wobble
}
