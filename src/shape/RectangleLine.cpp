#include "RectangleLine.h"

RectangleLine::RectangleLine(vec2 start, vec2 end,string label):
start(start),
end(end),
label(label){
	loadFonts();  // Load font dari AbstractShape
	maxProgress = totalSegments;  // Set max progress untuk isComplete()
}

void RectangleLine::setStart(float startX, float startY) {
	start = vec2(startX, startY);
}

void RectangleLine::setEnd(float endX, float endY) {
	end = vec2(endX, endY);
}

void RectangleLine::setLabel(std::string label) {
	this->label = label;
}

void RectangleLine::showDot() {
	dotVisible = true;
}

void RectangleLine::hideDot() {
	dotVisible = false;
}

void RectangleLine::showLabel() {
	labelVisible = true;
}

void RectangleLine::hideLabel() {
	labelVisible = false;
}

void RectangleLine::update() {
	if (showing) {
		if (progress < totalSegments) progress += speed;
	}
	else {
		if (progress > 0) progress -= speed;
	}
}

void RectangleLine::draw() {
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
		if (dotVisible) {
			ofDrawCircle(end.x, end.y, lineWidth * 2);
		}

		ofSetColor(0);
		if (labelVisible) {
			fontNormal.drawString(label, end.x + 10, end.y - 10);
		}
	}
	ofPopMatrix();
}