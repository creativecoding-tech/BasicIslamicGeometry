#include "RectangleLine.h"
#include "DotInfo.h"

RectangleLine::RectangleLine(vec2 start, vec2 end, vec2 intersec1, vec2 intersec2, string label1, string label2, float radius):
start(start),
end(end),
intersec1(intersec1),
intersec2(intersec2),
label1(label1),
label2(label2),
radius(radius),
originalRadius(radius)  // Simpan original radius
{
	loadFonts();  // Load font dari AbstractShape
	maxProgress = totalSegments;  // Set max progress untuk isComplete()
}

void RectangleLine::setIntersec1(vec2 intersec1) {
	this->intersec1 = intersec1;
}

void RectangleLine::setIntersec2(vec2 intersec2) {
	this->intersec2 = intersec2;
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

void RectangleLine::setRadius(float r) {
	// Re-calculate secara proporsional
	float scaleFactor = r / originalRadius;
	start = start * scaleFactor;
	end = end * scaleFactor;
	intersec1 = intersec1 * scaleFactor;
	intersec2 = intersec2 * scaleFactor;
	radius = r;
	originalRadius = r;  // Update originalRadius untuk scaling berikutnya
}

void RectangleLine::update(float deltaTime) {
	if (showing) {
		if (progress < totalSegments) progress += speed * deltaTime;
	}
	else {
		if (progress > 0) progress -= speed * deltaTime;
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
		// Gambar 2 dot di intersection points
		if (dotVisible) {
			// Dot pertama di intersection F→G ∩ C→E
			ofDrawCircle(intersec1.x, intersec1.y, lineWidth * 2);
			// Dot kedua di intersection F→G ∩ B→E
			ofDrawCircle(intersec2.x, intersec2.y, lineWidth * 2);
		}

		ofSetColor(0);
		// Gambar 2 label
		if (labelVisible) {
			fontNormal.drawString(label1, intersec1.x - 10, intersec1.y - 15);  
			fontNormal.drawString(label2, intersec2.x - 10, intersec2.y - 15); 
		}
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
void RectangleLine::addDotsToCache(std::vector<DotInfo>& dots) {
	// RectangleLine punya 2 dots: intersec1 dan intersec2
	dots.push_back({intersec1, "Rectangle"});
	dots.push_back({intersec2, "Rectangle"});
}