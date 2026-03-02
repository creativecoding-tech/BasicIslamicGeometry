#pragma once

#include "AbstractAnimation.h"

/**
 * RotateRightAnimation - Polygon rotate to RIGHT (clockwise) dan kembali
 *
 * Behavior: Osilasi halus dari 0° → +maxAngle → 0°
 * - Menggunakan half-cosine wave untuk smooth animation
 * - maxAngle: Amplitudo rotasi (derajat), default 90 derajat
 * - Durasi: ±2 detik untuk 1 siklus penuh
 * - Direction: Clockwise (positif)
 * - ⭐ NEW: Pause feature - Pause at maxAngle position for specified duration
 */
class RotateRightAnimation : public AbstractAnimation {
public:
  explicit RotateRightAnimation(float maxAngle = 90.0f);

  // Update rotasi: Osilasi ke kanan lalu kembali ke 0
  void update(float deltaTime) override;

  // Get current rotation angle in degrees
  float getAngle() const;

  // Reset animation ke awal
  void reset() override;

  // ⭐ NEW: Set pause duration (0.1 - 1.0 seconds)
  void setPauseDuration(float duration);
  float getPauseDuration() const;

private:
  float currentAngle; // Current rotation angle (derajat)
  float maxAngle;    // Amplitudo maksimum rotasi
  float time;        // Waktu berjalan untuk animasi
  float pauseDuration; // ⭐ NEW: Durasi pause di posisi maxAngle (seconds)
  float pauseTimer;   // ⭐ NEW: Timer untuk tracking pause
  bool isPaused;      // ⭐ NEW: Flag untuk status pause
  enum class RotatePhase {
    FORWARD,  // Dari 0° ke +maxAngle
    PAUSED,   // Pause di +maxAngle
    BACKWARD  // Dari +maxAngle kembali ke 0°
  };
  RotatePhase phase; // ⭐ NEW: Phase tracking
};
