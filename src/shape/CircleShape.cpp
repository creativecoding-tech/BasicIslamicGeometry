#include "CircleShape.h"
#include <ofMain.h>

CircleShape::CircleShape(float r) {
	radius = r;
}

void CircleShape::show() {
	showing = true;
}

void CircleShape::hide() {
	showing = false;
}

void CircleShape::update() {
	if (showing) {
		//Animasi muncul
		if (circleProgress < totalSegments) circleProgress += circleSpeed;
	}
	else {
		//Animasi hilang (reverse)
		if (circleProgress > 0) circleProgress -= circleSpeed;
	}
}

void CircleShape::draw() {
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(4);
	ofPolyline polyline;
	for (int i = 0; i <= circleProgress;i++) {
		float angle = ofMap(i, 0, totalSegments, 0, TWO_PI);
		float x = cos(angle) * radius;
		float y = sin(angle) * radius;
		polyline.addVertex(x, y);
	}
	polyline.close();
	polyline.draw();
	if (showing && circleProgress >= totalSegments) {
		ofFill();
		ofDrawCircle(0, 0, 10);
	}
}
