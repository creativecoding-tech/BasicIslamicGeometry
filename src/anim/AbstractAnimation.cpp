#include "AbstractAnimation.h"

//--------------------------------------------------------------
AbstractAnimation::AbstractAnimation(float speed)
    : progress(0.0f), speed(speed), baseSpeed(speed) {}

//--------------------------------------------------------------
bool AbstractAnimation::isComplete() const { return progress >= 1.0f; }

//--------------------------------------------------------------
void AbstractAnimation::reset() { progress = 0.0f; }

//--------------------------------------------------------------
float AbstractAnimation::getProgress() const { return progress; }

//--------------------------------------------------------------
void AbstractAnimation::setSpeed(float speed) {
  this->speed = speed;
  this->baseSpeed = speed; // Update base speed too when setting absolute speed
}

//--------------------------------------------------------------
void AbstractAnimation::setSpeedMultiplier(float multiplier) {
  this->speed = this->baseSpeed * multiplier;
}

//--------------------------------------------------------------
float AbstractAnimation::getSpeed() const { return speed; }
