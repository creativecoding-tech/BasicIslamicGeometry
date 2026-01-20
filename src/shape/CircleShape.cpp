#include "CircleShape.h"
#include <ofMain.h>

CircleShape::CircleShape(float r,std::string label,float posX,float posY) {
	radius = r;
	this->label = label;
	loadFonts();  // Load font dari AbstractShape
	this->posX = posX;
	this->posY = posY;
	maxProgress = totalSegments;  // Set max progress untuk isComplete()
}

void CircleShape::setLabel(std::string label) {
	this->label = label;
}

void CircleShape::setPosition(float x, float y) {
	this->posX = x;
	this->posY = y;
}

void CircleShape::update() {
	if (showing) {
		//Animasi muncul
		if (progress < totalSegments) progress += speed;
	}
	else {
		//Animasi hilang (reverse)
		if (progress > 0) progress -= speed;
	}
}

void CircleShape::draw() {
	ofPushMatrix();
	ofTranslate(posX, posY);

	ofNoFill();
	ofSetColor(0);
	ofSetLineWidth(lineWidth);

	ofPolyline polyline;

	// Hitung berapa banyak segment yang harus digambar
	int segmentsToDraw = (int)progress;

	// Gambar arc - dari angle 0 sampai circle progress
	for (int i = 0; i <= segmentsToDraw; i++) {
		// Map dari 0-100 ke 0-TWO_PI, tapi tambahkan sedikit buffer
		float angle = ofMap(i, 0, totalSegments, 0, TWO_PI);

		// Kalau ini adalah segment terakhir dan kita sudah full, pastikan angle = 0 (tutup loop)
		if (i == totalSegments && progress >= totalSegments) {
			angle = 0;  // Kembali ke titik awal
		}

		float x = cos(angle) * radius;
		float y = sin(angle) * radius;
		polyline.addVertex(x, y);
	}
	polyline.close();
	polyline.draw();

	if (showing && progress >= totalSegments) {
		ofFill();
		ofDrawCircle(0, 0, lineWidth*2);
		ofSetColor(0);
		if (useThin) {
			fontNormal.drawString(label, 10, -5);  // Font normal/tipis
		} else {
			fontBold.drawString(label, 10, -5);    // Font tebal
		}
	}
	ofPopMatrix();
}
