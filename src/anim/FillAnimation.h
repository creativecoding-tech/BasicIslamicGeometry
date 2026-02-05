#pragma once

#include "AbstractAnimation.h"

/**
 * FillAnimation - Animation filling DIRECTIONAL (bawah ke atas)
 * GENERIC: bisa dipakai untuk SEMUA shape type (PolygonShape, CustomLine, dll)
 *
 * Cara kerja:
 * - getWaterLevel() - level air (0.0 = kosong/bawah, 1.0 = penuh/atas)
 * - getWaveOffset(x) - wave offset horizontal untuk efek gelombang di permukaan air
 *
 * Animation mulai dari 50% (setengah) ke 100% (penuh)
 * Air mengisi dari bawah ke atas dengan wave horizontal di permukaan
 */
class FillAnimation : public AbstractAnimation {
public:
    // Constructor
    // waveAmplitude: besar gelombang di permukaan air (pixel)
    // waveFrequency: frekuensi gelombang
    FillAnimation(float waveAmplitude = 10.0f, float waveFrequency = 3.0f, float speed = 0.02f);

    // Update animation progress
    void update() override;

    // Ambil water level saat ini (0.0 = kosong/bawah, 1.0 = penuh/atas)
    float getWaterLevel() const;

    // Ambil wave offset horizontal
    // x: posisi X dalam pixel
    // Return: wave offset Y (naik/turun)
    float getWaveOffset(float x) const;

    // Setter untuk wave amplitude
    void setWaveAmplitude(float amplitude);
    float getWaveAmplitude() const;

    // Setter untuk wave frequency
    void setWaveFrequency(float frequency);
    float getWaveFrequency() const;

    // Override reset
    void reset() override;

private:
    float waveAmplitude;   // Besar gelombang (pixel)
    float waveFrequency;   // Frekuensi gelombang
};
