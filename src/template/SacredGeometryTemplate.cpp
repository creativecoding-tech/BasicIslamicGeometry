#include "SacredGeometryTemplate.h"

//--------------------------------------------------------------
void SacredGeometryTemplate::startSequentialDrawing() {
	// Cek apakah semua shapes sudah visible/showing
	bool allVisible = true;
	for (auto &shape : shapes) {
		if (!shape->showing) {
			allVisible = false;
			break;
		}
	}

	if (allVisible) {
		// Semua shapes sudah visible, jangan jalankan sequential
		return;
	}

	// Cek apakah ada shape yang sedang drawing (belum complete)
	bool stillDrawing = false;
	for (auto &shape : shapes) {
		if (!shape->isComplete()) {
			stillDrawing = true;
			break;
		}
	}

	if (stillDrawing) {
		// Ada shape yang masih drawing, jangan jalankan sequential
		return;
	}

	// Cek apakah sequential sudah pernah selesai
	if (sequentialCompleted) {
		// Sequential sudah selesai sebelumnya, jangan jalankan lagi
		return;
	}

	// Reset semua shapes ke hidden
	for (auto &shape : shapes) {
		shape->hide();
	}

	// Mulai sequential mode
	sequentialMode = true;
	sequentialCompleted = false;
	currentShapeIndex = 0;

	// Set sequential mode untuk semua shapes
	for (auto &shape : shapes) {
		shape->setSequentialMode(sequentialMode);
	}

	// Show shape pertama
	if (!shapes.empty()) {
		shapes[0]->show();
	}
}

//--------------------------------------------------------------
bool SacredGeometryTemplate::updateSequentialDrawing() {
	if (!sequentialMode) return true;

	AbstractShape* currentShape = nullptr;

	// Tentukan shape berdasarkan index
	if (currentShapeIndex >= 0 && currentShapeIndex < shapes.size()) {
		currentShape = shapes[currentShapeIndex].get();
	}

	// Cek jika current shape sudah complete
	if (currentShape && currentShape->isComplete()) {
		// Pindah ke shape berikutnya
		currentShapeIndex++;

		// Show shape berikutnya jika masih ada
		if (currentShapeIndex < shapes.size()) {
			shapes[currentShapeIndex]->show();
			return false;  // Belum selesai
		} else {
			// Semua shapes sudah complete, matikan sequential mode dan tandai selesai
			sequentialMode = false;
			sequentialCompleted = true;
			return true;  // Selesai!
		}
	}

	return false;  // Belum selesai
}

//--------------------------------------------------------------
void SacredGeometryTemplate::showAllShapes() {
	// Matikan sequential mode (paralel mode)
	sequentialMode = false;

	// Set mode untuk semua shapes (pakai sequentialMode yang sudah false)
	for (auto &shape : shapes) {
		shape->setSequentialMode(sequentialMode);
	}

	// Show semua shapes (akan animasi barengan/paralel)
	for (auto &shape : shapes) {
		shape->show();
	}

	// Reset sequential completed flag agar bisa jalankan lagi
	sequentialCompleted = false;
}

//--------------------------------------------------------------
void SacredGeometryTemplate::hideAllShapes() {
	// Matikan sequential mode
	sequentialMode = false;

	// Set paralel mode untuk semua shapes (pakai sequentialMode yang sudah false)
	for (auto &shape : shapes) {
		shape->setSequentialMode(sequentialMode);
	}

	// Hide semua shapes
	for (auto &shape : shapes) {
		shape->hide();
	}

	// Reset sequential completed flag agar bisa jalankan lagi
	sequentialCompleted = false;
}

//--------------------------------------------------------------
void SacredGeometryTemplate::toggleLabels() {
	// Iterate semua template shapes
	for (auto &shape : shapes) {
		if (labelsVisible) {
			shape->showLabel();
		} else {
			shape->hideLabel();
		}
	}
}

//--------------------------------------------------------------
void SacredGeometryTemplate::toggleDots() {
	// Iterate semua template shapes
	for (auto &shape : shapes) {
		if (dotsVisible) {
			shape->showDot();
		} else {
			shape->hideDot();
		}
	}
}

//--------------------------------------------------------------
void SacredGeometryTemplate::setLineWidth(float width) {
	lineWidth = width;

	// Update line width untuk semua shapes
	for (auto& shape : shapes) {
		shape->setLineWidth(lineWidth);
	}
}

//--------------------------------------------------------------
void SacredGeometryTemplate::updateLineWidth(float width) {
	lineWidth = width;

	// Update line width untuk semua shapes (realtime, tanpa rebuild)
	for (auto& shape : shapes) {
		shape->setLineWidth(lineWidth);
	}
}
