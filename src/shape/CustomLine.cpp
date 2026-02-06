#include "CustomLine.h"

//--------------------------------------------------------------
CustomLine::CustomLine()
	: lineWidth(3.0f)
	, curve(0.0f)
	, progress(1.0f)
	, speed(1.2f)  // Delta time calibrated (0.02f * 60 FPS)
	, selected(false)
	, color(ofColor(0, 0, 255)) {  // Default biru
}

//--------------------------------------------------------------
CustomLine::CustomLine(vector<vec2> points, ofColor color, float lineWidth)
	: points(points)
	, color(color)
	, lineWidth(lineWidth)
	, curve(0.0f)
	, progress(1.0f)
	, speed(1.2f)  // Delta time calibrated (0.02f * 60 FPS)
	, selected(false) {
}

//--------------------------------------------------------------
void CustomLine::draw() const {
	if (points.size() < 2) return;

	vec2 start = points[0];
	vec2 end = points[1];

	// Set warna: selalu pakai original color (tidak berubah saat selected)
	ofSetColor(color);

	ofSetLineWidth(lineWidth);

	// Gambar lurus atau curved
	if (curve != 0.0f) {
		drawCurvedLine(start, end);
	} else {
		drawStraightLine(start, end);
	}
}

//--------------------------------------------------------------
void CustomLine::drawStraightLine(vec2 start, vec2 end) const {
	// Gambar lurus dengan polar coordinates (progressive)
	float totalAngle = atan2(end.y - start.y, end.x - start.x);
	float totalDistance = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));

	float totalSegments = 100.0f;
	ofPolyline polyline;
	int segmentsToDraw = static_cast<int>(totalSegments * progress);

	for (int j = 0; j <= segmentsToDraw; j++) {
		float t = ofMap(j, 0, totalSegments, 0, 1);
		float currentDist = totalDistance * t;

		float x = start.x + cos(totalAngle) * currentDist;
		float y = start.y + sin(totalAngle) * currentDist;

		polyline.addVertex(x, y);
	}

	if (!polyline.getVertices().empty()) {
		polyline.draw();
	}
}

//--------------------------------------------------------------
void CustomLine::drawCurvedLine(vec2 start, vec2 end) const {
	// Gambar bezier curve dengan quadratic bezier
	vec2 controlPoint = calculateControlPoint(start, end);

	ofPolyline bezierPoly;
	int segments = 100;
	int segmentsToDraw = static_cast<int>(segments * progress);

	for (int j = 0; j <= segmentsToDraw; j++) {
		float t = (float)j / segments;
		// Quadratic bezier formula: (1-t)²p0 + 2(1-t)t*p1 + t²p2
		// untuk curva yang smooth
		vec2 point =
			start * (1 - t) * (1 - t) +
			controlPoint * 2 * (1 - t) * t +
			end * t * t;
		bezierPoly.addVertex(point.x, point.y);
	}

	if (!bezierPoly.getVertices().empty()) {
		bezierPoly.draw();
	}
}

//--------------------------------------------------------------
vec2 CustomLine::calculateControlPoint(vec2 start, vec2 end) const {
	// Hitung control point untuk quadratic bezier
	vec2 midPoint = (start + end) / 2.0f;
	vec2 dir = end - start;

	// Perpendicular vector (-y, x)
	vec2 perp = vec2(-dir.y, dir.x);

	// Normalize perpendicular
	float perpLen = glm::length(perp);
	if (perpLen > 0) {
		perp = perp / perpLen;
	}

	// Control point = midpoint + curve * perpendicular
	return midPoint + perp * curve;
}

//--------------------------------------------------------------
void CustomLine::setPoints(const vector<vec2>& points) {
	this->points = points;
}

//--------------------------------------------------------------
void CustomLine::setColor(ofColor color) {
	this->color = color;
}

//--------------------------------------------------------------
void CustomLine::setLineWidth(float width) {
	this->lineWidth = width;
}

//--------------------------------------------------------------
void CustomLine::setCurve(float curve) {
	this->curve = curve;
}

//--------------------------------------------------------------
void CustomLine::setProgress(float progress) {
	this->progress = progress;
}

//--------------------------------------------------------------
void CustomLine::setSpeed(float speed) {
	this->speed = speed;
}

//--------------------------------------------------------------
void CustomLine::setSelected(bool selected) {
	this->selected = selected;
}

//--------------------------------------------------------------
void CustomLine::updateProgress(float deltaTime) {
	if (progress < 1.0f) {
		progress += speed * deltaTime;
		if (progress > 1.0f) {
			progress = 1.0f;
		}
	}
}

//--------------------------------------------------------------
vector<vec2> CustomLine::getSampledPoints() const {
	if (points.size() < 2) return vector<vec2>();

	vec2 start = points[0];
	vec2 end = points[1];

	// Kalau curve = 0, return 2 titik saja (lurus)
	if (curve == 0.0f) {
		return {start, end};
	}

	// Kalau curve != 0, ambil 100 sampled points dari bezier curve
	vector<vec2> sampledPoints;
	vec2 controlPoint = calculateControlPoint(start, end);
	int segments = 100;

	for (int j = 0; j <= segments; j++) {
		float t = (float)j / segments;
		vec2 point =
			start * (1 - t) * (1 - t) +
			controlPoint * 2 * (1 - t) * t +
			end * t * t;
		sampledPoints.push_back(point);
	}

	return sampledPoints;
}
