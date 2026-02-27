#pragma once

#include "AbstractAnimation.h"

/**
 * RotateLeftAnimation - Animasi rotasi berlawanan arah jarum jam
 * PolygonShape akan diputar pada titik tengah bounding box-nya
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

private:
  float currentAngle;    // Sudut saat ini dalam derajat
  float maxAngle;        // Amplitudo rotasi maksimum
  float time;            // Waktu simulasi untuk osilasi
  bool speedInitialized; // Freeze status speed
};
