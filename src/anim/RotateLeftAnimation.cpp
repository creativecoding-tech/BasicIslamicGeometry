#include "RotateLeftAnimation.h"
#include <cmath>

// Constructor
// maxAngle: Amplitudo maksimum rotasi ke kiri (derajat). Default 90 derajat.
RotateLeftAnimation::RotateLeftAnimation(float maxAngle)
    : AbstractAnimation(1.0f), currentAngle(0.0f), maxAngle(maxAngle),
      time(0.0f), speedInitialized(false) {
  // speed (multiplier) diatur melalui setSpeedMultiplier setelah pembuatan
}

void RotateLeftAnimation::setSpeedMultiplier(float multiplier) {
  if (!speedInitialized) {
    AbstractAnimation::setSpeedMultiplier(multiplier);
    speedInitialized = true;
  }
}

// Update rotasi: Osilasi ke kiri lalu kembali ke 0
void RotateLeftAnimation::update(float deltaTime) {
  if (isComplete())
    return;

  // Gunakan speed multiplier dari AbstractAnimation
  time += deltaTime * speed;

  if (time >= 2.0f) {
    // Selesai satu siklus penuh
    time = 2.0f;
    currentAngle = 0.0f;
    progress = 1.0f; // Tandai animasi selesai
  } else {
    // Gunakan half-cosine wave untuk osilasi mulus dari 0 ke -maxAngle lalu
    // kembalike 0 (1.0f - cos(time * 2.0f * PI)) / 2.0f menghasilkan range 0.0
    // -> 1.0 -> 0.0
    float factor = (1.0f - std::cos(time * 3.14159265f)) / 2.0f;
    currentAngle = factor * -maxAngle;
  }
}

float RotateLeftAnimation::getAngle() const { return currentAngle; }

void RotateLeftAnimation::reset() {
  AbstractAnimation::reset();
  currentAngle = 0.0f;
  time = 0.0f;
}
