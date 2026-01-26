#include "FadeInAnimation.h"

//--------------------------------------------------------------
FadeInAnimation::FadeInAnimation(float targetAlpha, float speed)
    : AbstractAnimation(speed)
    , targetAlpha(targetAlpha) {
}

//--------------------------------------------------------------
void FadeInAnimation::update() {
    if (progress < 1.0f) {
        progress += speed;
        if (progress > 1.0f) {
            progress = 1.0f;
        }
    }
}

//--------------------------------------------------------------
float FadeInAnimation::getCurrentAlpha() const {
    // Alpha = targetAlpha * progress (linear fade-in)
    return targetAlpha * progress;
}

//--------------------------------------------------------------
void FadeInAnimation::setTargetAlpha(float targetAlpha) {
    this->targetAlpha = targetAlpha;
}

//--------------------------------------------------------------
float FadeInAnimation::getTargetAlpha() const {
    return targetAlpha;
}
