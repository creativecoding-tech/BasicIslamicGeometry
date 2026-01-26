#pragma once

#include "AbstractAnimation.h"

/**
 * FadeInAnimation - Animation fade-in untuk alpha/blending
 * Cocok untuk: PolygonShape, fill areas, dll
 */
class FadeInAnimation : public AbstractAnimation {
public:
    // Constructor
    FadeInAnimation(float targetAlpha = 255.0f, float speed = 0.02f);

    // Update animation progress
    void update() override;

    // Ambil current alpha berdasarkan progress (0 - targetAlpha)
    float getCurrentAlpha() const;

    // Set target alpha (maximum alpha)
    void setTargetAlpha(float targetAlpha);
    float getTargetAlpha() const;

private:
    float targetAlpha;  // Target alpha (default 255)
};
