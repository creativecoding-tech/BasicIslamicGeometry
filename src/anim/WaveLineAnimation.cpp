#include "WaveLineAnimation.h"
#include <cmath>

//--------------------------------------------------------------
WaveLineAnimation::WaveLineAnimation(float amplitude, float frequency, float speed)
    : AbstractAnimation(speed), amplitude(amplitude), frequency(frequency) {
}

//--------------------------------------------------------------
void WaveLineAnimation::update(float deltaTime) {
    // Infinite animation: progress terus bertambah, reset ketika lewat dari 1.0
    progress += speed * deltaTime;
    if (progress > 1.0f) {
        progress -= 1.0f;  // Loop back to 0.0 (infinite wave)
    }
}

//--------------------------------------------------------------
glm::vec2 WaveLineAnimation::getWaveOffset(float t, glm::vec2 direction) const {
    // Wave effect sepanjang line:
    // - t: posisi sepanjang line (0.0 = start, 1.0 = end)
    // - direction: normalized direction vector line
    // - progress: waktu wave bergerak (0.0 - 1.0 loop)

    // 1. Hitung phase berdasarkan posisi (t) dan waktu (progress)
    //    Wave formula: sin(2π * (frequency * t + progress))
    float phase = frequency * TWO_PI * (t - progress);

    // 2. Hitung wave magnitude (-1.0 sampai 1.0)
    float waveMagnitude = std::sin(phase);

    // 3. Hitung perpendicular vector (normal) dari line direction
    //    Jika direction = (dx, dy), normal = (-dy, dx)
    glm::vec2 normal = glm::vec2(-direction.y, direction.x);

    // 4. Normalize normal vector
    float normalLength = glm::length(normal);
    if (normalLength > 0.0f) {
        normal /= normalLength;
    }

    // 5. Return offset vector: normal * waveMagnitude * amplitude
    return normal * waveMagnitude * amplitude;
}

//--------------------------------------------------------------
void WaveLineAnimation::setAmplitude(float amplitude) {
    this->amplitude = amplitude;
}

//--------------------------------------------------------------
float WaveLineAnimation::getAmplitude() const {
    return amplitude;
}

//--------------------------------------------------------------
void WaveLineAnimation::setFrequency(float frequency) {
    this->frequency = frequency;
}

//--------------------------------------------------------------
float WaveLineAnimation::getFrequency() const {
    return frequency;
}

//--------------------------------------------------------------
void WaveLineAnimation::reset() {
    AbstractAnimation::reset();
    // Tidak ada additional state untuk wave animation
}
