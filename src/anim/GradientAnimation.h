#pragma once

#include "AbstractAnimation.h"
#include "ofMain.h"

// Gradient Animation - Smooth gradient flow effect pada warna fill
// Polygon statis (tidak bergerak), tapi warna fill mengalir seperti gradient
// Finite animation: berhenti setelah duration (default 5 detik)
class GradientAnimation : public AbstractAnimation {
public:
    GradientAnimation(float speed = 2.0f, float frequency = 5.0f, float duration = 5.0f);
    void update(float deltaTime = 0.016f) override;

    float getSpeed() const;
    void setSpeed(float speed);

    float getFrequency() const;
    void setFrequency(float frequency);

    float getDuration() const;
    void setDuration(float duration);

    float getTime() const;  // Ambil accumulated time

    void reset() override;

private:
    float speed;      // Speed dari gradient flow (default 2.0)
    float frequency;  // Spatial frequency dari gradient pattern (default 5.0)
    float duration;   // Durasi animation dalam detik (default 5.0)
    float time;       // Accumulated time
};
