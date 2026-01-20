#include "CrossLine.h"
#include "ofMain.h"

CrossLine::CrossLine(vec2 start, vec2 end,string label1,string label2):
	start(start),
	end(end),
	label1(label1),
	label2(label2)
{
	this->fontNormal.load("C:\\Windows\\Fonts\\calibri.ttf", 15);  // Font tipis
	this->fontBold.load("C:\\Windows\\Fonts\\calibrib.ttf", 20);   // Font bold
}

void CrossLine::setLabel1(string lbl) {
	label1 = lbl;
}

void CrossLine::setLabel2(string lbl) {
	label2 = lbl;
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
		//dot or circle posisi pada intersection crossline and circle A
		// Hitung posisi dot pada perpotongan lingkaran A (radius 240) dengan garis
		//Koordinate polar trigonometri
		float angle = atan2(end.y - start.y, end.x - start.x);
		float dotX = cos(angle) * 240;
		float dotY = sin(angle) * 240;
		ofDrawCircle(dotX, dotY, lineWidth * 2);

		//Dot or circle end x and y
		ofDrawCircle(end.x, end.y, lineWidth * 2);

		ofSetColor(0);

		if (useThin) {
			fontNormal.drawString(label1, dotX, dotY-10);  // Font normal/tipis
			fontNormal.drawString(label2, end.x - 20, end.y - 5);
		}
		else {
			fontBold.drawString(label1, dotX, dotY-10);    // Font tebal
			fontBold.drawString(label2, end.x - 20, end.y - 5);
		}
	}
	ofPopMatrix();
}