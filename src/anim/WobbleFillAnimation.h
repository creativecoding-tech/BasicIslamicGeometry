#pragma once

#include "AbstractAnimation.h"

/**
 * WobbleFillAnimation - Polygon shape bergerak-goyang DENGAN gradual fill
 * Mirip WOBBLE tapi fill-nya bertambah perlahan dari 0% ke target (default 50%)
 *
 * Cara kerja:
 * - getWobbleOffset() - offset untuk menggoyang polygon vertices
 * - getFillLevel() - level fill saat ini (0.0 -> 1.0 gradual)
 * - Polygon bergerak-goyang, fill bertambah perlahan seperti air yang naik
 */
class WobbleFillAnimation : public AbstractAnimation {
public:
    // Constructor
    // wobbleAmount:berapa besar polygon digoyang (default 30.0)
    // wobbleSpeed: kecepatan goyang (default 5.0)
    // wobbleFrequency: frekuensi goyang (default 1.8)
    // targetFillLevel: level fill target (1.0 = 100%, default 1.0)
    // fillSpeed: kecepatan fill bertambah (default 0.3)
    WobbleFillAnimation(float wobbleAmount = 30.0f, float wobbleSpeed = 5.0f,
                       float wobbleFrequency = 1.8f, float targetFillLevel = 1.0f,
                       float fillSpeed = 0.3f);

    // Update animation progress dengan deltaTime
    void update(float deltaTime = 0.016f) override;

    // Ambil wobble offset untuk polygon vertices
    glm::vec2 getWobbleOffset() const;

    // Ambil fill level (0.0 - 1.0)
    float getFillLevel() const;

    // Setter dan getter
    void setWobbleAmount(float amount);
    float getWobbleAmount() const;

    void setWobbleSpeed(float speed);
    float getWobbleSpeed() const;

    void setWobbleFrequency(float freq);
    float getWobbleFrequency() const;

    void setTargetFillLevel(float level);
    float getTargetFillLevel() const;

    void setFillSpeed(float speed);
    float getFillSpeed() const;

    // Override reset
    void reset() override;

private:
    float wobbleAmount;        // Besar wobble (vertex offset)
    float wobbleSpeed;         // Kecepatan wobble
    float wobbleFrequency;     // Frekuensi wobble
    float targetFillLevel;     // Target fill level (0.0 - 1.0)
    float currentFillLevel;    // Current fill level (bertambah perlahan)
    float fillSpeed;           // Kecepatan fill bertambah
    float time;                // Accumulated time
};
