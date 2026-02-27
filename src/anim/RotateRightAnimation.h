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

private:
  float currentAngle; // Current rotation angle (derajat)
  float maxAngle;    // Amplitudo maksimum rotasi
  float time;        // Waktu berjalan untuk animasi
};
