#include "ParallelogramLine.h"
#include "DotInfo.h"

ParallelogramLine::ParallelogramLine(vec2 start, vec2 end, vec2 intersecCrossLine, std::string label, float radius) :
	start(start),
	end(end),
	intersecCrossLine(intersecCrossLine),
	label(label),
	radius(radius),
	originalRadius(radius)  // Simpan original radius
{
	loadFonts();  // Load font dari AbstractShape
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

void ParallelogramLine::setLabel(std::string label) {
	this->label = label;
}

void ParallelogramLine::setRadius(float r) {
	// Re-calculate secara proporsional
	float scaleFactor = r / originalRadius;
	start = start * scaleFactor;
	end = end * scaleFactor;
	intersecCrossLine = intersecCrossLine * scaleFactor;
	radius = r;
	originalRadius = r;  // Update originalRadius untuk scaling berikutnya
}

void ParallelogramLine::showDot() {
	dotVisible = true;
}

void ParallelogramLine::hideDot() {
	dotVisible = false;
}

void ParallelogramLine::showLabel() {
	labelVisible = true;
}

void ParallelogramLine::hideLabel() {
	labelVisible = false;
}

void ParallelogramLine::update(float deltaTime) {
	if (showing) {
		if (progress < totalSegments) progress += speed * deltaTime;
	}
	else {
		if (progress > 0) progress -= speed * deltaTime;
	}
}

void ParallelogramLine::draw() {
	ofPushMatrix();
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(lineWidth);

	// Polar Thinking untuk line drawing
	// Hitung angle dan distance dari start ke end
	float totalAngle = atan2(end.y - start.y, end.x - start.x);
	float totalDistance = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));

	ofPolyline polyline;
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
		// Gambar dot hanya jika dotVisible = true
		if (dotVisible) {
			// Dot pada intersection dengan crossline
			ofDrawCircle(intersecCrossLine.x, intersecCrossLine.y, lineWidth * 2);
		}

		ofSetColor(0);

		// Gambar label jika labelVisible = true
		if (labelVisible) {
			fontNormal.drawString(label, intersecCrossLine.x - 10, intersecCrossLine.y - 15);
		}
	}
	ofPopMatrix();

}

//--------------------------------------------------------------
void ParallelogramLine::addDotsToCache(std::vector<DotInfo>& dots) {
	// ParallelogramLine hanya punya satu dot: intersecCrossLine
	dots.push_back({intersecCrossLine, "Parallelogram"});
}
