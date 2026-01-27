#include "OctagramLine.h"
#include "DotInfo.h"
OctagramLine::OctagramLine(vec2 start, vec2 end, std::optional<vec2> nextPoint, string label) :
	start(start),
	end(end),
	nextPoint(nextPoint),
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
		if (isSequentialMode) {
			// SEQUENTIAL MODE: Phase 1 dulu, baru Phase 2
			if (progress < totalSegments) {
				progress += speed;
			}
			else if (nextPoint.has_value() && extensionProgress < totalSegments) {
				extensionProgress += speed;
			}
		}
		else {
			// PARALEL MODE: Kedua line berjalan barengan
			if (progress < totalSegments) {
				progress += speed;
			}
			if (nextPoint.has_value() && extensionProgress < totalSegments) {
				extensionProgress += speed;
			}
		}
	}
	else {
		if (isSequentialMode) {
			// SEQUENTIAL MODE: Hide extension dulu, baru main line
			if (extensionProgress > 0) {
				extensionProgress -= speed;
			}
			else if (progress > 0) {
				progress -= speed;
			}
		}
		else {
			// PARALEL MODE: Kedua line hide barengan
			if (extensionProgress > 0) {
				extensionProgress -= speed;
			}
			if (progress > 0) {
				progress -= speed;
			}
		}
	}
}

bool OctagramLine::isComplete() {
	if (showing) {
		// Jika ada nextPoint, harus ngecek kedua progress (main + extension)
		if (nextPoint.has_value()) {
			return (progress >= totalSegments) && (extensionProgress >= totalSegments);
		}
		// Jika tidak ada nextPoint, cuma ngecek main progress
		else {
			return progress >= totalSegments;
		}
	}
	else {
		// Saat hiding, complete jika kedua progress sudah 0
		if (nextPoint.has_value()) {
			return (progress <= 0) && (extensionProgress <= 0);
		}
		else {
			return progress <= 0;
		}
	}
}

void OctagramLine::draw() {
	ofPushMatrix();
	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(lineWidth);

	// === PHASE 1: Main line (start → end) - SELALU DIGAMBAR ===
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

	// === PHASE 2: Extension line (end → nextPoint) - HANYA jika nextPoint ada ===
	if (nextPoint.has_value()) {
		vec2 np = nextPoint.value();

		float totalAngle2 = atan2(np.y - end.y, np.x - end.x);
		float totalDistance2 = sqrt(pow(np.x - end.x, 2) + pow(np.y - end.y, 2));

		ofPolyline polyline2;
		// Gambar extension line secara progressif menggunakan extensionProgress
		for (int i = 0; i <= extensionProgress; i++) {
			float t = ofMap(i, 0, totalSegments, 0, 1);
			float currentDist = totalDistance2 * t;
			float x = end.x + cos(totalAngle2) * currentDist;
			float y = end.y + sin(totalAngle2) * currentDist;
			polyline2.addVertex(x, y);
		}
		polyline2.close();
		polyline2.draw();
	}

	// === DOT & LABEL ===
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

//--------------------------------------------------------------
void OctagramLine::addDotsToCache(std::vector<DotInfo>& dots) {
	// OctagramLine punya satu dot: end point
	dots.push_back({end, "Octagram"});
}
