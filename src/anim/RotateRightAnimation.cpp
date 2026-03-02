#include "RotateRightAnimation.h"
#include <cmath>

// Constructor
// maxAngle: Amplitudo maksimum rotasi ke kanan (derajat). Default 90 derajat.
RotateRightAnimation::RotateRightAnimation(float maxAngle)
    : AbstractAnimation(1.0f), currentAngle(0.0f), maxAngle(maxAngle),
      time(0.0f), pauseDuration(0.0f), pauseTimer(0.0f), isPaused(false),
      phase(RotatePhase::FORWARD) {
  // speed (multiplier) diatur melalui setSpeedMultiplier setelah pembuatan
}

// Update rotasi: Osilasi ke kanan lalu kembali ke 0
void RotateRightAnimation::update(float deltaTime) {
  if (isComplete())
    return;

  // ⭐ NEW: Handle pause phase
  if (isPaused) {
    pauseTimer += deltaTime * speed;

    // Cek jika pause selesai
    if (pauseTimer >= pauseDuration) {
      isPaused = false;
      pauseTimer = 0.0f;
      phase = RotatePhase::BACKWARD;
      // Reset time untuk backward phase (dari 1.0 kembali ke 2.0)
      time = 1.0f;
    } else {
      // Masih pause, angle tetap di maxAngle
      currentAngle = maxAngle;
      return; // Skip update normal
    }
  }

  // Gunakan speed multiplier dari AbstractAnimation
  time += deltaTime * speed;

  // ⭐ NEW: Phase-based animation
  if (phase == RotatePhase::FORWARD) {
    // Forward phase: 0° → +maxAngle
    if (time >= 1.0f) {
      // Mencapai maxAngle, mulai pause jika ada durasi pause
      if (pauseDuration > 0.0f) {
        isPaused = true;
        phase = RotatePhase::PAUSED;
        currentAngle = maxAngle;
        time = 1.0f; // Clamp time
        return;
      } else {
        // Tidak ada pause, langsung ke backward
        phase = RotatePhase::BACKWARD;
        time = 1.0f;
      }
    } else {
      // Masih forward: 0 → +maxAngle
      // Gunakan half-cosine wave: 0.0 → 1.0
      float factor = (1.0f - std::cos(time * 3.14159265f)) / 2.0f;
      currentAngle = factor * maxAngle;
    }
  }

  if (phase == RotatePhase::BACKWARD) {
    // Backward phase: +maxAngle → 0°
    if (time >= 2.0f) {
      // Selesai satu siklus penuh
      time = 2.0f;
      currentAngle = 0.0f;
      progress = 1.0f; // Tandai animasi selesai
    } else {
      // Masih backward: +maxAngle → 0
      // Gunakan half-cosine wave: 1.0 → 0.0
      float factor = (1.0f - std::cos((time - 1.0f) * 3.14159265f)) / 2.0f;
      currentAngle = (1.0f - factor) * maxAngle; // Reverse: +maxAngle → 0
    }
  }
}

float RotateRightAnimation::getAngle() const { return currentAngle; }

void RotateRightAnimation::setPauseDuration(float duration) {
  pauseDuration = duration;
}

float RotateRightAnimation::getPauseDuration() const {
  return pauseDuration;
}

void RotateRightAnimation::reset() {
  AbstractAnimation::reset();
  currentAngle = 0.0f;
  time = 0.0f;
  pauseTimer = 0.0f;
  isPaused = false;
  phase = RotatePhase::FORWARD;
}
