#pragma once

#include "ofMain.h"

/**
 * AbstractAnimation - Base class untuk semua animation di project ini
 * Bisa dipakai untuk: PolygonShape, CustomLine, AbstractShape, dll
 */
class AbstractAnimation {
public:
    // Constructor
    AbstractAnimation(float speed = 0.02f);
    virtual ~AbstractAnimation() = default;

    // Update animation progress (dipanggil setiap frame)
    virtual void update() = 0;

    // Cek apakah animation sudah selesai
    bool isComplete() const;

    // Reset animation ke awal
    virtual void reset();

    // Getter untuk progress (0.0 - 1.0)
    float getProgress() const;

    // Setter untuk speed
    void setSpeed(float speed);
    float getSpeed() const;

protected:
    float progress;  // 0.0 = belum mulai, 1.0 = selesai
    float speed;     // Kecepatan animation (increment per frame)
};
