#pragma once

#include "AbstractAnimation.h"

/**
 * SpinRightAnimation - Polygon berputar pada porosnya sendiri (clockwise)
 *
 * Behavior: Polygon SPIN pada center/centroid-nya sendiri
 * - Beda dengan Orbit: Orbit mengelilingi (0,0), Spin berputar pada porosnya
 * - Osilasi halus dari 0° → +maxAngle → 0°
 * - maxAngle: Amplitudo spin (derajat), default 90 derajat
 * - Durasi: ±2 detik untuk 1 siklus penuh
 * - Direction: Clockwise (positif)
 */
class SpinRightAnimation : public AbstractAnimation {
public:
  // Constructor
  explicit SpinRightAnimation(float maxAngle = 90.0f); // default 90 derajat

  // Update spin progress
  void update(float deltaTime) override;

  // Get current spin angle in degrees
  float getAngle() const;

  // Reset animation ke awal
  void reset() override;

private:
  float currentAngle; // Current spin angle (derajat)
  float maxAngle;     // Amplitudo maksimum spin
  float time;         // Waktu berjalan untuk animasi
};
