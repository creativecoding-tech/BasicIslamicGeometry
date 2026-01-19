#include "CrossLine.h"
#include "ofMain.h"

CrossLine::CrossLine(vec2 start, vec2 end,string label):
	start(start),
	end(end),
	label(label){
	this->fontNormal.load("C:\\Windows\\Fonts\\calibri.ttf", 15);  // Font tipis
	this->fontBold.load("C:\\Windows\\Fonts\\calibrib.ttf", 20);   // Font bold
}

void CrossLine::setLabel(string lbl) {
	label = lbl;
}

void CrossLine::show() {
	showing = true;
}

void CrossLine::hide() {
	showing = false;
}

void CrossLine::update() {
	if (showing) {
		if (progress < totalSegements) progress += speed;
	}
	else {
		if (progress > 0)progress -= speed;
	}
}

void CrossLine::setStart(float startX, float startY) {
	start = vec2(startX,startY);
}
void CrossLine::setEnd(float endX, float endY) {
	end = vec2(endX, endY);
}

void CrossLine::setLineWidth(float width) {
	lineWidth = width;
}

void CrossLine::setThin(bool thin) {
	useThin = thin;
}

void CrossLine::draw() {
	ofPushMatrix();
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(lineWidth);

	ofPolyline polyline;
	//Gambar garis secara progressif
	for (int i = 0;i <= progress;i++) {
		float t = ofMap(i, 0, totalSegements, 0, 1);
		float x = ofLerp(start.x, end.x, t);
		float y = ofLerp(start.y, end.y, t);

		polyline.addVertex(x, y);
	}
	polyline.close();
	polyline.draw();

	if (showing && progress >= totalSegements) {
		ofFill();
		ofDrawCircle(end.x, end.y, lineWidth * 2);
		ofSetColor(0);

		if (useThin) {
			fontNormal.drawString(label, end.x-20, end.y-5);  // Font normal/tipis
		}
		else {
			fontBold.drawString(label, end.x-20, end.y-5);    // Font tebal
		}
	}
	ofPopMatrix();
}