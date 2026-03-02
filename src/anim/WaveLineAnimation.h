#pragma once

#include "AbstractAnimation.h"

/**
 * WaveLineAnimation - Wave effect animation untuk CustomLine
 * Gelombang bergerak sepanjang line (dari start ke end)
 * Infinite animation (tidak ada complete, terus bergerak)
 *
 * Cara pakai:
 * - CustomLine: modify vertex positions dengan getWaveOffset(float t, vec2 direction)
 * - t parameter (0.0 - 1.0) adalah posisi sepanjang line
 * - direction parameter adalah normalized direction vector line
 * - Wave effect infinite (progress 0.0 → 1.0 → loop ke 0.0 lagi)
 */
class WaveLineAnimation : public AbstractAnimation {
public:
    // Constructor
    // amplitude: tinggi wave (pixel) - halus: 2-5px
    // frequency: berapa banyak wave (1-3 untuk halus)
    // speed: kecepatan wave bergerak (1.0-3.0)
    WaveLineAnimation(float amplitude = 3.0f, float frequency = 2.0f, float speed = 2.0f);

    // Update animation progress (infinite loop)
    void update(float deltaTime = 0.016f) override;

    // Ambil wave offset untuk vertex di posisi t (0.0 - 1.0) sepanjang line
    // direction: normalized direction vector of line (untuk hitung perpendicular)
    // Return perpendicular offset (vec2) untuk ditambahkan ke vertex position
    glm::vec2 getWaveOffset(float t, glm::vec2 direction) const;

    // Setter untuk amplitude (tinggi wave dalam pixel)
    void setAmplitude(float amplitude);
    float getAmplitude() const;

    // Setter untuk frequency (berapa banyak wave)
    void setFrequency(float frequency);
    float getFrequency() const;

    // Override reset untuk reset wave state
    void reset() override;

private:
    float amplitude;   // Tinggi wave (pixel) - halus: 2-5px
    float frequency;   // Frekuensi wave (berapa banyak gelombang) - halus: 1-3
};
