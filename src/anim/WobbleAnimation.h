#pragma once

#include "AbstractAnimation.h"

/**
 * WobbleAnimation - Animation wobble/goyang untuk shapes
 * Fleksibel: bisa dipakai untuk PolygonShape, CircleShape, AbstractShape, dll
 *
 * Cara pakai:
 * - Polygon: modify vertices dengan getWobbleOffset()
 * - Circle: modify radius dengan getWobbleScale()
 * - Generic: apply transformation dengan getWobbleFactor()
 */
class WobbleAnimation : public AbstractAnimation {
public:
    // Constructor
    // amplitude: besar wobble dalam pixel
    // frequency: berapa banyak wobble per animation
    WobbleAnimation(float amplitude = 20.0f, float frequency = 5.0f, float speed = 0.02f);

    // Update animation progress
    void update() override;

    // Ambil wobble factor (-1.0 sampai 1.0) berdasarkan progress
    // Bisa dipakai untuk scaling, rotation, dll
    float getWobbleFactor() const;

    // Ambil wobble offset (vec2) untuk vertex manipulation
    // Berguna untuk polygon vertices perturbation
    glm::vec2 getWobbleOffset() const;

    // Ambil wobble scale untuk radius modification
    // Berguna untuk circle, dll
    float getWobbleScale() const;

    // Setter untuk amplitude (besar wobble dalam pixel)
    void setAmplitude(float amplitude);
    float getAmplitude() const;

    // Setter untuk frequency (berapa kali wobble per animation)
    void setFrequency(float frequency);
    float getFrequency() const;

    // Override reset untuk reset wobble state
    void reset() override;

private:
    float amplitude;   // Besar wobble (pixel)
    float frequency;   // Frekuensi wobble (berapa banyak goyang)
};
