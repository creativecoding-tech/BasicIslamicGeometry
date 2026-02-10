#include "FillAnimation.h"
#include <cmath>

//--------------------------------------------------------------
FillAnimation::FillAnimation(float waveAmplitude, float waveFrequency, float speed)
    : AbstractAnimation(speed), waveAmplitude(waveAmplitude), waveFrequency(waveFrequency) {
}

//--------------------------------------------------------------
void FillAnimation::update(float deltaTime) {
    if (progress < 1.0f) {
        progress += speed * deltaTime;
        if (progress > 1.0f) {
            progress = 1.0f;
        }
    }
}

//--------------------------------------------------------------
float FillAnimation::getWaterLevel() const {
    // Animation mulai dari BAWAH (0%) ke ATAS (100%)
    // progress 0.0 → 0.0 (kosong/bawah)
    // progress 1.0 → 1.0 (penuh/atas)
    return progress;
}


//--------------------------------------------------------------
float FillAnimation::getWaveOffset(float x) const {
    // Wave amplitude decay hanya saat 90% penuh atau lebih
    float decay = 1.0f;
    if (progress > 0.9f) {
        float over90 = (progress - 0.9f) / 0.1f;
        decay = 1.0f - over90;
    }

    // Horizontal wave: sin(x * frequency + time * phase)
    // x dalam pixel, waveFrequency mengontrol spatial frequency
    float wave = std::sin((x * waveFrequency * 0.01f) + progress * waveFrequency * PI * 2.0f);

    // Return Y offset (vertical displacement)
    float offset = wave * waveAmplitude * decay;

    return offset;
}

//--------------------------------------------------------------
void FillAnimation::setWaveAmplitude(float amplitude) {
    this->waveAmplitude = amplitude;
}

//--------------------------------------------------------------
float FillAnimation::getWaveAmplitude() const {
    return waveAmplitude;
}

//--------------------------------------------------------------
void FillAnimation::setWaveFrequency(float frequency) {
    this->waveFrequency = frequency;
}

//--------------------------------------------------------------
float FillAnimation::getWaveFrequency() const {
    return waveFrequency;
}

//--------------------------------------------------------------
void FillAnimation::reset() {
    AbstractAnimation::reset();
    // Tidak ada additional state
}
