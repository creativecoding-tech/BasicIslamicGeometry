#include "ParallelogramLine.h"

ParallelogramLine::ParallelogramLine(vec2 start, vec2 end, vec2 intersecCrossLine) :
	start(start),
	end(end),
	intersecCrossLine(intersecCrossLine) {
	maxProgress = totalSegments;  // Set max progress untuk isComplete()
}

void ParallelogramLine::setStart(float startX, float startY) {
	start = vec2(startX, startY);
}

void ParallelogramLine::setEnd(float endX, float endY) {
	end = vec2(endX, endY);
}

void ParallelogramLine::setIntersecCrossLine(vec2 intersec) {
	intersecCrossLine = intersec;
}

void ParallelogramLine::showDot() {
	dotVisible = true;
}

void ParallelogramLine::hideDot() {
	dotVisible = false;
}

void ParallelogramLine::update() {
	if (showing) {
		if (progress < totalSegments) progress += speed;
	}
	else {
		if (progress > 0) progress -= speed;
	}
}

void ParallelogramLine::draw() {
	ofPushMatrix();
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(lineWidth);
	ofPolyline polyline;
	for (int i = 0; i <= progress; i++) {

		float t = ofMap(i, 0, totalSegments, 0, 1);
		float x = ofLerp(start.x, end.x, t);
		float y = ofLerp(start.y, end.y, t);

		polyline.addVertex(x, y);
	}
	polyline.close();
	polyline.draw();
	if (showing && progress >= totalSegments) {
		ofFill();
		// Gambar dot hanya jika dotVisible = true
		if (dotVisible) {
			// Dot pada intersection dengan crossline
			ofDrawCircle(intersecCrossLine.x, intersecCrossLine.y, lineWidth * 2);
		}
	}
	ofPopMatrix();

}
