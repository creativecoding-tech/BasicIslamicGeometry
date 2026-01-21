#include "OctagramLine.h"
OctagramLine::OctagramLine(vec2 start, vec2 end, string label) :
	start(start),
	end(end),
	label(label){
	loadFonts();  // Load font dari AbstractShape
	maxProgress = totalSegments;  // Set max progress untuk isComplete()
}

void OctagramLine::showDot() {
	dotVisible = true;
}

void OctagramLine::hideDot() {
	dotVisible = false;
}

void OctagramLine::showLabel() {
	labelVisible = true;
}

void OctagramLine::hideLabel() {
	labelVisible = false;
}

void OctagramLine::update() {
	if (showing) {
		if (progress < totalSegments) progress += speed;
	}
	else {
		if (progress > 0) progress -= speed;
	}
}

void OctagramLine::draw() {
	ofPushMatrix();
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(lineWidth);

	// Polar Thinking untuk line drawing
	// Hitung angle dan distance dari start ke end
	float totalAngle = atan2(end.y - start.y, end.x - start.x);
	float totalDistance = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));

	ofPolyline polyline;
	// Gambar garis secara progressif menggunakan polar coordinates
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
		// Gambar dot di end point
		if (dotVisible) {
			ofDrawCircle(end.x, end.y, lineWidth * 2);
		}

		ofSetColor(0);
		// Gambar label di end point
		if (labelVisible) {
			fontNormal.drawString(label, end.x + 10, end.y - 15);
		}
	}
	ofPopMatrix();
}
