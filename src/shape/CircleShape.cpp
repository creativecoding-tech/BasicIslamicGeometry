#include "CircleShape.h"
#include <ofMain.h>

CircleShape::CircleShape(float r,std::string label,float posX,float posY) {
	radius = r;
	this->label = label;
	this->fontNormal.load("C:\\Windows\\Fonts\\calibri.ttf", 15);  // Font tipis
	this->fontBold.load("C:\\Windows\\Fonts\\calibrib.ttf", 20);   // Font bold
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

void CircleShape::setLineWidth(float width) {
	lineWidth = width;
}

void CircleShape::setThin(bool thin) {
	useThin = thin;
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
	ofSetLineWidth(lineWidth);
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
		ofDrawCircle(0, 0, lineWidth*2);
		ofSetColor(0);
		if (useThin) {
			fontNormal.drawString(label, 10, -5);  // Font normal/tipis
		} else {
			fontBold.drawString(label, 10, -5);    // Font tebal
		}
	}
	ofPopMatrix();
}
