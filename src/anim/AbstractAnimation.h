#pragma once

#include "ofMain.h"

/**
 * AbstractAnimation - Base class untuk semua animation di project ini
 * Bisa dipakai untuk: PolygonShape, CustomLine, AbstractShape, dll
 */
class AbstractAnimation {
public:
  // Constructor
  AbstractAnimation(
      float speed = 1.2f); // Delta time calibrated (0.02f * 60 FPS)
  virtual ~AbstractAnimation() = default;

  // Update animation progress (dipanggil setiap frame)
  virtual void update(float deltaTime = 0.016f) = 0;

  // Cek apakah animation sudah selesai
  bool isComplete() const;

  // Reset animation ke awal
  virtual void reset();

  // Getter untuk progress (0.0 - 1.0)
  float getProgress() const;

  // Setter untuk speed
  void setSpeed(float speed);
  float getSpeed() const;

  // Set speed multiplier (updates speed relative to baseSpeed)
  void setSpeedMultiplier(float multiplier);

protected:
  float progress;  // 0.0 = belum mulai, 1.0 = selesai
  float speed;     // Kecepatan animation (increment per frame)
  float baseSpeed; // Kecepatan dasar (intrinsic) animation
};
