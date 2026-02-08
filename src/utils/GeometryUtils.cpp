#include "GeometryUtils.h"

//--------------------------------------------------------------
// COORDINATE TRANSFORMATION
//--------------------------------------------------------------

vec2 GeometryUtils::screenToWorld(vec2 screenPos, vec2 pan, float zoom, float rotation) {
	// Inverse transform untuk mengkonversi screen coordinate ke world coordinate
	// Urutan transform di draw():
	// 1. Translate center
	// 2. Rotate
	// 3. Scale/Zoom
	// 4. Pan
	//
	// Inverse transform (screen to world):
	// 1. Translate screen pos relative to center
	// 2. Inverse rotate (-rotation)
	// 3. Inverse scale (1/zoom)
	// 4. Inverse pan (-pan)

	vec2 pos = screenPos;

	// 1. Translate relative to center
	pos.x -= ofGetWidth() / 2.0f;
	pos.y -= ofGetHeight() / 2.0f;

	// 2. Inverse rotate
	float rotRad = ofDegToRad(-rotation);
	float cosR = cos(rotRad);
	float sinR = sin(rotRad);
	float newX = pos.x * cosR - pos.y * sinR;
	float newY = pos.x * sinR + pos.y * cosR;
	pos.x = newX;
	pos.y = newY;

	// 3. Inverse scale
	pos.x /= zoom;
	pos.y /= zoom;

	// 4. Inverse pan
	pos.x -= pan.x;
	pos.y -= pan.y;

	return pos;
}

//--------------------------------------------------------------
// LINE OPERATIONS
//--------------------------------------------------------------

float GeometryUtils::distanceToLine(vec2 point, vec2 lineStart, vec2 lineEnd, float curve) {
	// Hitung jarak terpendek dari point ke garis/lengkungan

	// Jika curve != 0, hitung jarak ke bezier curve
	if (curve != 0.0f) {
		// Hitung control point
		vec2 midPoint = (lineStart + lineEnd) / 2.0f;
		vec2 dir = lineEnd - lineStart;
		vec2 perp = vec2(-dir.y, dir.x);
		float perpLen = glm::length(perp);
		if (perpLen > 0) {
			perp = perp / perpLen;
		}
		vec2 controlPoint = midPoint + perp * curve;

		// Sampling bezier curve untuk cari jarak minimum
		float minDistance = 999999.0f;
		int samples = 50;

		for (int i = 0; i <= samples; i++) {
			float t = (float)i / samples;
			// Quadratic bezier
			vec2 curvePoint = lineStart * (1 - t) * (1 - t) +
						controlPoint * 2 * (1 - t) * t + lineEnd * t * t;

			float distance = glm::length(point - curvePoint);
			if (distance < minDistance) {
				minDistance = distance;
			}
		}

		return minDistance;
	} else {
		// Hitung jarak ke garis lurus (original code)
		vec2 lineVec = lineEnd - lineStart;
		vec2 pointVec = point - lineStart;
		float lineLengthSq = glm::dot(lineVec, lineVec);

		if (lineLengthSq == 0)
			return 0.0f;

		// Project pointVec ke lineVec (hasilnya parameter t dalam [0,1])
		float projection = glm::dot(pointVec, lineVec) / lineLengthSq;

		// Clamp projection ke line segment (biar tidak extend)
		projection = ofClamp(projection, 0.0f, 1.0f);

		vec2 closestPoint = lineStart + lineVec * projection;

		// Hitung jarak dari point ke closestPoint
		return glm::length(point - closestPoint);
	}
}

bool GeometryUtils::isMouseOverLine(vec2 mousePos, vec2 lineStart, vec2 lineEnd, float lineWidth) {
	// Hitung jarak dari titik ke garis (point-to-line distance)
	// Formula: jarak = |(end - start) × (start - point)| / |end - start|
	vec2 lineVec = lineEnd - lineStart;
	vec2 pointVec = mousePos - lineStart;
	float lineLength = glm::length(lineVec);

	if (lineLength == 0)
		return false;

	// Project pointVec ke lineVec
	float projection = glm::dot(pointVec, lineVec) / lineLength;
	vec2 closestPoint = lineStart + (lineVec / lineLength) * projection;

	// Clamp projection ke line segment (biar tidak extend)
	if (projection < 0)
		closestPoint = lineStart;
	if (projection > 1)
		closestPoint = lineEnd;

	// Hitung jarak dari mousePos ke closestPoint di garis
	float distance = glm::length(mousePos - closestPoint);

	// Threshold = setengah line width + sedikit buffer
	return distance <= (lineWidth / 2.0f) + 2.0f;
}

int GeometryUtils::getLineIndexAtPosition(vec2 mousePos, const std::vector<CustomLine>& lines, float lineWidth) {
	// Cari garis dengan JARAK TERDEKAT
	int closestLineIndex = -1;
	float closestDistance = 999999.0f;

	for (int i = 0; i < lines.size(); i++) {
		const CustomLine &line = lines[i];
		const vector<vec2> &points = line.getPoints();
		if (points.size() >= 2) {
			vec2 start = points[0];
			vec2 end = points[1];

			// Skip invalid lines (start == end)
			if (glm::length(start - end) < 1.0f) {
				continue;
			}

			// Hitung jarak ke garis ini (dengan curve support)
			float distance = distanceToLine(mousePos, start, end, line.getCurve());

			// Update jika lebih dekat
			if (distance < closestDistance) {
				closestDistance = distance;
				closestLineIndex = i;
			}
		}
	}

	// Return line terdekat (jika dalam threshold)
	if (closestDistance <= (lineWidth / 2.0f) + 2.0f) {
		return closestLineIndex;
	}

	return -1; // Tidak ada yang diklik
}

bool GeometryUtils::lineExists(vec2 from, vec2 to, const std::vector<CustomLine>& lines) {
	// Cek apakah ada line dengan titik awal dan akhir yang sama
	// Mencegah duplicate line creation
	for (auto &line : lines) {
		const vector<vec2> &points = line.getPoints();
		if (points.size() >= 2) {
			vec2 lineStart = points.front();
			vec2 lineEnd = points.back();

			// Cek both directions (from→to OR to→from)
			if ((glm::length(lineStart - from) < 1.0f &&
				 glm::length(lineEnd - to) < 1.0f) ||
				(glm::length(lineStart - to) < 1.0f &&
					glm::length(lineEnd - from) < 1.0f)) {
				return true;
			}
		}
	}
	return false;
}

//--------------------------------------------------------------
// POINT OPERATIONS
//--------------------------------------------------------------

bool GeometryUtils::isMouseOverDot(vec2 mousePos, vec2 dotPos, float threshold) {
	// Euclidean distance - hitung jarak antara 2 titik
	float distance = glm::length(mousePos - dotPos);
	return distance <= threshold;
}

float GeometryUtils::distanceToPoint(vec2 p1, vec2 p2) {
	// Euclidean distance between two points
	return glm::length(p1 - p2);
}
