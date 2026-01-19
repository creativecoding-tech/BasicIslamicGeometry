#include "CircleShape.h"
#include <ofMain.h>

CircleShape::CircleShape(float r,std::string label,float posX,float posY) {
	radius = r;
	this->label = label;
	this->font.load("C:\\Windows\\Fonts\\arial.ttf", 20);
	this->posX = posX;
	this->posY = posY;
}

void CircleShape::show() {
	showing = true;
}

void CircleShape::hide() {
	showing = false;
}

void CircleShape::setLabel(std::string label) {
	this->label = label;
}

void CircleShape::setPosition(float x, float y) {
	this->posX = posX;
	this->posY = posY;
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
	ofPushMatrix();
	ofTranslate(posX, posY);
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
		font.drawString(label, 10, -5);
	}
	ofPopMatrix();
}
