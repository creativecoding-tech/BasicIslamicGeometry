#include "CrossLine.h"
#include <ofMain.h>

CrossLine::CrossLine(vec2 start, vec2 end, string label1, string label2, float radius):
	start(start),
	end(end),
	label1(label1),
	label2(label2),
	radius(radius)
{
	loadFonts();  // Load font dari AbstractShape
	maxProgress = totalSegments;  // Set max progress untuk isComplete()
}

void CrossLine::setLabel1(string lbl) {
	label1 = lbl;
}

void CrossLine::setLabel2(string lbl) {
	label2 = lbl;
}

void CrossLine::setStart(float startX, float startY) {
	start = vec2(startX, startY);
}

void CrossLine::setEnd(float endX, float endY) {
	end = vec2(endX, endY);
}

void CrossLine::showLabel() {
	labelVisible = true;
}

void CrossLine::hideLabel() {
	labelVisible = false;
}

void CrossLine::showDot() {
	dotVisible = true;
}

void CrossLine::hideDot() {
	dotVisible = false;
}

void CrossLine::update() {
	if (showing) {
		if (progress < totalSegments) progress += speed;
	}
	else {
		if (progress > 0) progress -= speed;
	}
}

void CrossLine::draw() {
	ofPushMatrix();
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(lineWidth);

	// Polar Thinking untuk line drawing
	// Hitung angle dan distance dari start ke end
	float totalAngle = atan2(end.y - start.y, end.x - start.x);
	float totalDistance = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));

	ofPolyline polyline;
	//Gambar garis secara progressif menggunakan polar coordinates
	for (int i = 0; i <= progress; i++) {
		float t = ofMap(i, 0, totalSegments, 0, 1);
		// Interpolate distance sepanjang angle yang sama
		float currentDist = totalDistance * t;
		// Konversi polar ke cartesian: x = start + cos(angle) * distance
		float x = start.x + cos(totalAngle) * currentDist;
		float y = start.y + sin(totalAngle) * currentDist;

		polyline.addVertex(x, y);
	}
	polyline.close();
	polyline.draw();

	if (showing && progress >= totalSegments) {
		ofFill();
		//dot or circle posisi pada intersection crossline
		// Gunakan totalAngle yang sudah dihitung untuk efisiensi
		float dotX = cos(totalAngle) * radius;
		float dotY = sin(totalAngle) * radius;

		// Gambar dot hanya jika dotVisible = true
		if (dotVisible) {
			ofDrawCircle(dotX, dotY, lineWidth * 2);
			//Dot or circle end x and y
			ofDrawCircle(end.x, end.y, lineWidth * 2);
		}

		ofSetColor(0);

		if (labelVisible) {
			fontNormal.drawString(label1, dotX, dotY-10);
			fontNormal.drawString(label2, end.x - 20, end.y - 5);
		}
	}
	ofPopMatrix();
}
