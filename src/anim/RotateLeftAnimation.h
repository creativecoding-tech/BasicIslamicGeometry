#pragma once

#include "AbstractAnimation.h"

/**
 * RotateLeftAnimation - Animasi rotasi berlawanan arah jarum jam
 * PolygonShape akan diputar pada titik tengah bounding box-nya
 * - ⭐ NEW: Pause feature - Pause at maxAngle position for specified duration
 */
class RotateLeftAnimation : public AbstractAnimation {
public:
  // Constructor
  RotateLeftAnimation(float maxAngle = 90.0f); // default 90 derajat maksimum

  // Update animation progress
  void update(float deltaTime = 0.016f) override;

  // Ambil rotasi saat ini dalam derajat
  float getAngle() const;

  // Override setSpeedMultiplier untuk freeze speed setelah draw
  void setSpeedMultiplier(float multiplier);

  // Override reset
  void reset() override;

  // ⭐ NEW: Set pause duration (0.1 - 1.0 seconds)
  void setPauseDuration(float duration);
  float getPauseDuration() const;

private:
  float currentAngle;    // Sudut saat ini dalam derajat
  float maxAngle;        // Amplitudo rotasi maksimum
  float time;            // Waktu simulasi untuk osilasi
  float pauseDuration;   // ⭐ NEW: Durasi pause di posisi maxAngle (seconds)
  float pauseTimer;      // ⭐ NEW: Timer untuk tracking pause
  bool isPaused;         // ⭐ NEW: Flag untuk status pause
  enum class RotatePhase {
    FORWARD,  // Dari 0° ke -maxAngle
    PAUSED,   // Pause di -maxAngle
    BACKWARD  // Dari -maxAngle kembali ke 0°
  };
  RotatePhase phase; // ⭐ NEW: Phase tracking
};
