#include "CartesianAxes.h"
#include <ofMain.h>

CartesianAxes::CartesianAxes(float r) {
	radius = r;
	speed = 1.2f;  // Delta time calibrated (0.02f * 60 FPS)
	maxProgress = maxScale;  // Set max progress untuk isComplete()
	loadFonts();  // Load font untuk label sudut
}

void CartesianAxes::update(float deltaTime) {
	if (showing) {
		if (progress < maxScale) {
			progress += speed * deltaTime;
		}
	}
	else {
		if (progress > 0) {
			progress -= speed * deltaTime;
		}
	}
}

void CartesianAxes::draw() {
	if (progress > 0) {
		float currentLength = radius * progress;
		ofNoFill();
		ofSetColor(0);
		ofSetLineWidth(lineWidth);
		ofDrawLine(-currentLength, 0, currentLength, 0);
		ofDrawLine(0, -currentLength, 0, currentLength);

		// Draw label sudut hanya jika complete dan labelVisible = true
		if (progress >= maxScale && labelVisible) {
			ofFill();
			ofSetColor(0);

			// Ujung kanan (0°)
			fontNormal.drawString("0 (0°)", currentLength + 10, 5);
			// Ujung bawah (PI/2 = 90°)
			fontNormal.drawString("PI/2 (90°)", -90, currentLength - 100);
			// Ujung kiri (PI = 180°)
			fontNormal.drawString("PI (180°)", -currentLength + 45, -20);
			// Ujung atas (-PI/2 = 270°)
			fontNormal.drawString("-PI/2 (270°)", -100, -currentLength + 100);
		}
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

void CartesianAxes::showLabel() {
	labelVisible = true;
}

void CartesianAxes::hideLabel() {
	labelVisible = false;
}

void CartesianAxes::setRadius(float r) {
	radius = r;  // CartesianAxes pakai radius langsung di draw(), jadi cukup assign
}
