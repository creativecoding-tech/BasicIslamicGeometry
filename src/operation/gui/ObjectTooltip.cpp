#include "ObjectTooltip.h"
#include "../ofApp.h"

ObjectTooltip::ObjectTooltip(ofApp* app) : app(app) {
}

//--------------------------------------------------------------
void ObjectTooltip::draw() {
	// Tampilkan tooltip untuk SEMUA selected objects
	// Menggunakan OpenFrameworks custom rendering (bukan ImGui)
	// agar bisa multiple tooltips sekaligus

	// Tooltip untuk selected userDots
	for (int index : app->selectedUserDotIndices) {
		drawUserDotTooltip(index);
	}

	// TODO: Tooltip untuk selected customLines
	for (int index : app->selectedLineIndices) {
		drawCustomLineTooltip(index);
	}

	// TODO: Tooltip untuk selected polygons
	for (int index : app->selectedPolygonIndices) {
		drawPolygonTooltip(index);
	}
}

//--------------------------------------------------------------
void ObjectTooltip::drawUserDotTooltip(int index) {
	// Safety check 1: Index bounds
	if (index < 0 || index >= app->userDots.size()) {
		return;
	}

	// Safety check 2: Null pointer
	auto& dot = app->userDots[index];
	if (!dot) {
		return;
	}

	vec2 dotPos = dot->getPosition();
	vec2 lowerBound = dot->getLowerBound();

	// Convert world coordinate ke screen coordinate
	// Transform: world → screen
	vec2 screenPos = dotPos;

	// 1. Apply canvas translation (pan)
	screenPos.x += app->canvasTranslation.x;
	screenPos.y += app->canvasTranslation.y;

	// 2. Apply zoom/scale
	screenPos.x *= app->canvasZoom;
	screenPos.y *= app->canvasZoom;

	// 3. Apply rotation
	float rotRad = ofDegToRad(app->canvasRotation);
	float cosR = cos(rotRad);
	float sinR = sin(rotRad);
	float rotatedX = screenPos.x * cosR - screenPos.y * sinR;
	float rotatedY = screenPos.x * sinR + screenPos.y * cosR;
	screenPos.x = rotatedX;
	screenPos.y = rotatedY;

	// 4. Translate to center screen
	screenPos.x += ofGetWidth() / 2.0f;
	screenPos.y += ofGetHeight() / 2.0f;

	// Safety check 3: Clamp screenPos ke dalam window bounds
	float screenWidth = ofGetWidth();
	float screenHeight = ofGetHeight();
	screenPos.x = ofClamp(screenPos.x, 0.0f, screenWidth - 1);
	screenPos.y = ofClamp(screenPos.y, 0.0f, screenHeight - 1);

	// Generate tooltip text
	string tooltipText;
	if (dotPos.x != lowerBound.x) {
		// Horizontal offset
		float offsetX = dotPos.x - lowerBound.x;
		tooltipText = "offset = " + ofToString(offsetX, 1) + " (dot[" + ofToString(index) + "])";
	} else {
		// Vertical offset
		float offsetY = dotPos.y - lowerBound.y;
		tooltipText = "offset = " + ofToString(offsetY, 1) + " (dot[" + ofToString(index) + "])";
	}

	// Draw tooltip menggunakan OpenFrameworks (bukan ImGui)
	ofPushStyle();

	// Background tooltip box
	ofRectangle textBounds = app->fontNormal.getStringBoundingBox(tooltipText, 0, 0);
	float paddingX = 10.0f;  // Horizontal padding
	float paddingY = 4.0f;   // Vertical padding (lebih compact)
	float boxWidth = textBounds.width + paddingX * 2;
	float boxHeight = textBounds.height + paddingY * 2;
	float boxX = screenPos.x + 15.0f;  // Offset ke kanan dari dot
	float boxY = screenPos.y - boxHeight / 2.0f;  // Center vertical

	// Semi-transparent dark background
	ofSetColor(0, 0, 0, 200);
	ofDrawRectangle(boxX, boxY, boxWidth, boxHeight);

	// Border
	ofSetColor(100, 100, 100, 255);
	ofNoFill();
	ofSetLineWidth(1.0f);
	ofDrawRectangle(boxX, boxY, boxWidth, boxHeight);

	// Text (vertically & horizontally centered)
	ofSetColor(255, 255, 255, 255);  // White text
	float textX = boxX + (boxWidth - textBounds.width) / 2;  // Horizontal center
	float textY = boxY + (boxHeight - textBounds.height) / 2 - textBounds.y;  // Vertical center
	app->fontNormal.drawString(tooltipText, textX, textY);

	ofPopStyle();
}

//--------------------------------------------------------------
void ObjectTooltip::drawCustomLineTooltip(int index) {
	// Safety check 1: Index bounds
	if (index < 0 || index >= app->customLines.size()) {
		return;
	}

	CustomLine& line = app->customLines[index];
	const vector<vec2>& points = line.getPoints();

	if (points.size() < 2) {
		return;
	}

	// Hitung midpoint line untuk tooltip position
	vec2 start = points[0];
	vec2 end = points[1];
	vec2 midPoint = (start + end) * 0.5f;

	// Convert world coordinate ke screen coordinate
	vec2 screenPos = midPoint;

	// 1. Apply canvas translation (pan)
	screenPos.x += app->canvasTranslation.x;
	screenPos.y += app->canvasTranslation.y;

	// 2. Apply zoom/scale
	screenPos.x *= app->canvasZoom;
	screenPos.y *= app->canvasZoom;

	// 3. Apply rotation
	float rotRad = ofDegToRad(app->canvasRotation);
	float cosR = cos(rotRad);
	float sinR = sin(rotRad);
	float rotatedX = screenPos.x * cosR - screenPos.y * sinR;
	float rotatedY = screenPos.x * sinR + screenPos.y * cosR;
	screenPos.x = rotatedX;
	screenPos.y = rotatedY;

	// 4. Translate to center screen
	screenPos.x += ofGetWidth() / 2.0f;
	screenPos.y += ofGetHeight() / 2.0f;

	// Generate tooltip text (2 baris)
	string line1 = line.getLabel();  // customLine0 atau DcustomLine0
	string line2 = "curve = " + ofToString(line.getCurve(), 2);

	// Hitung box size untuk 2 baris text
	ofRectangle bounds1 = app->fontNormal.getStringBoundingBox(line1, 0, 0);
	ofRectangle bounds2 = app->fontNormal.getStringBoundingBox(line2, 0, 0);
	float maxWidth = max(bounds1.width, bounds2.width);
	float totalHeight = bounds1.height + bounds2.height + 1.0f;  // 1px gap antar baris (lebih rapat)

	float paddingX = 10.0f;
	float paddingY = 18.0f;  // Vertical padding lebih besar untuk 2 baris
	float boxWidth = maxWidth + paddingX * 2;
	float boxHeight = totalHeight + paddingY * 2;
	float boxX = screenPos.x + 15.0f;  // Offset ke kanan dari midpoint
	float boxY = screenPos.y - boxHeight / 2.0f;  // Center vertical

	// Draw tooltip box
	ofPushStyle();

	// Semi-transparent dark background
	ofSetColor(0, 0, 0, 200);
	ofDrawRectangle(boxX, boxY, boxWidth, boxHeight);

	// Border
	ofSetColor(100, 100, 100, 255);
	ofNoFill();
	ofSetLineWidth(1.0f);
	ofDrawRectangle(boxX, boxY, boxWidth, boxHeight);

	// Draw 2 baris text (centered horizontally)
	ofSetColor(255, 255, 255, 255);  // White text

	// Baris 1: Label (customLine0)
	float text1X = boxX + (boxWidth - bounds1.width) / 2;
	float text1Y = boxY + paddingY - 6.0f - bounds1.y;  // Naikkan 6px
	app->fontNormal.drawString(line1, text1X, text1Y);

	// Baris 2: curve = value
	float text2X = boxX + (boxWidth - bounds2.width) / 2;
	float text2Y = text1Y + bounds1.height + 1.0f - bounds2.y;  // 1px gap (lebih rapat)
	app->fontNormal.drawString(line2, text2X, text2Y);

	ofPopStyle();
}

//--------------------------------------------------------------
void ObjectTooltip::drawPolygonTooltip(int index) {
	// Safety check 1: Index bounds
	if (index < 0 || index >= app->polygonShapes.size()) {
		return;
	}

	PolygonShape& polygon = app->polygonShapes[index];
	const vector<vec2>& vertices = polygon.getVertices();

	if (vertices.empty()) {
		return;
	}

	// Hitung centroid polygon untuk tooltip position
	vec2 centroid(0.0f, 0.0f);
	for (const vec2& v : vertices) {
		centroid += v;
	}
	centroid /= (float)vertices.size();

	// Convert world coordinate ke screen coordinate
	vec2 screenPos = centroid;

	// 1. Apply canvas translation (pan)
	screenPos.x += app->canvasTranslation.x;
	screenPos.y += app->canvasTranslation.y;

	// 2. Apply zoom/scale
	screenPos.x *= app->canvasZoom;
	screenPos.y *= app->canvasZoom;

	// 3. Apply rotation
	float rotRad = ofDegToRad(app->canvasRotation);
	float cosR = cos(rotRad);
	float sinR = sin(rotRad);
	float rotatedX = screenPos.x * cosR - screenPos.y * sinR;
	float rotatedY = screenPos.x * sinR + screenPos.y * cosR;
	screenPos.x = rotatedX;
	screenPos.y = rotatedY;

	// 4. Translate to center screen
	screenPos.x += ofGetWidth() / 2.0f;
	screenPos.y += ofGetHeight() / 2.0f;

	// Generate tooltip text (2 baris)
	string line1 = "polygon[" + ofToString(index) + "]";
	string line2 = "vertices = " + ofToString(vertices.size());

	// Hitung box size untuk 2 baris text
	ofRectangle bounds1 = app->fontNormal.getStringBoundingBox(line1, 0, 0);
	ofRectangle bounds2 = app->fontNormal.getStringBoundingBox(line2, 0, 0);
	float maxWidth = max(bounds1.width, bounds2.width);
	float totalHeight = bounds1.height + bounds2.height + 0.0f;  // 0px gap (lebih rapat)

	float paddingX = 10.0f;
	float paddingY = 18.0f;  // Vertical padding untuk 2 baris
	float boxWidth = maxWidth + paddingX * 2;
	float boxHeight = totalHeight + paddingY * 2;
	float boxX = screenPos.x + 15.0f;  // Offset ke kanan dari centroid
	float boxY = screenPos.y - boxHeight / 2.0f;  // Center vertical

	// Draw tooltip box
	ofPushStyle();

	// Semi-transparent dark background
	ofSetColor(0, 0, 0, 200);
	ofDrawRectangle(boxX, boxY, boxWidth, boxHeight);

	// Border
	ofSetColor(100, 100, 100, 255);
	ofNoFill();
	ofSetLineWidth(1.0f);
	ofDrawRectangle(boxX, boxY, boxWidth, boxHeight);

	// Draw 2 baris text (centered horizontally)
	ofSetColor(255, 255, 255, 255);  // White text

	// Baris 1: polygon[index]
	float text1X = boxX + (boxWidth - bounds1.width) / 2;
	float text1Y = boxY + paddingY - 9.0f - bounds1.y;  // Offset 9px ke atas
	app->fontNormal.drawString(line1, text1X, text1Y);

	// Baris 2: vertices = count
	float text2X = boxX + (boxWidth - bounds2.width) / 2;
	float text2Y = text1Y + bounds1.height + 0.0f - bounds2.y;  // 0px gap (lebih rapat)
	app->fontNormal.drawString(line2, text2X, text2Y);

	ofPopStyle();
}
