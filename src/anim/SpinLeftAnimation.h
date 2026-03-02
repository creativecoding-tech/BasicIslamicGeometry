#pragma once

#include "AbstractAnimation.h"

/**
 * SpinLeftAnimation - Polygon berputar pada porosnya sendiri (counter-clockwise)
 *
 * Behavior: Polygon SPIN pada center/centroid-nya sendiri
 * - Beda dengan Orbit: Orbit mengelilingi (0,0), Spin berputar pada porosnya
 * - Osilasi halus dari 0° → -maxAngle → 0°
 * - maxAngle: Amplitudo spin (derajat), default 90 derajat
 * - Durasi: ±2 detik untuk 1 siklus penuh
 * - Direction: Counter-clockwise (negatif)
 * - ⭐ NEW: Pause feature - Pause at maxAngle position for specified duration
 */
class SpinLeftAnimation : public AbstractAnimation {
public:
  // Constructor
  explicit SpinLeftAnimation(float maxAngle = 90.0f); // default 90 derajat

  // Update spin progress
  void update(float deltaTime) override;

  // Get current spin angle in degrees
  float getAngle() const;

  // Reset animation ke awal
  void reset() override;

  // ⭐ NEW: Set pause duration (0.1 - 1.0 seconds)
  void setPauseDuration(float duration);
  float getPauseDuration() const;

private:
  float currentAngle; // Current spin angle (derajat)
  float maxAngle;     // Amplitudo maksimum spin
  float time;         // Waktu berjalan untuk animasi
  float pauseDuration; // ⭐ NEW: Durasi pause di posisi maxAngle (seconds)
  float pauseTimer;    // ⭐ NEW: Timer untuk tracking pause
  bool isPaused;       // ⭐ NEW: Flag untuk status pause
  enum class SpinPhase {
    FORWARD,  // Dari 0° ke -maxAngle
    PAUSED,   // Pause di -maxAngle
    BACKWARD  // Dari -maxAngle kembali ke 0°
  };
  SpinPhase phase; // ⭐ NEW: Phase tracking
};
