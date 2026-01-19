#include "CartesianAxes.h"
#include <ofMain.h>

CartesianAxes::CartesianAxes(float r) {
	radius = r;
}

void CartesianAxes::show() {
	showing = true;
}

void CartesianAxes::hide() {
	showing = false;
}

void CartesianAxes::update() {
	if (showing) {
		if (progress < maxScale) {
			progress += speed;
		}
	}
	else {
		if (progress > 0) {
			progress -= speed;
		}
	}
}

void CartesianAxes::draw() {
	if (progress > 0) {
		float currentLength = radius * progress;
		ofNoFill();
		ofSetColor(0);
		ofSetLineWidth(2);
		ofDrawLine(-currentLength, 0, currentLength, 0);
		ofDrawLine(0, -currentLength, 0, currentLength);
	}
}

bool CartesianAxes::isComplete() {
	if (showing) {
		return progress >= maxScale;
	}
	else {
		return progress <= 0;
	}
}


