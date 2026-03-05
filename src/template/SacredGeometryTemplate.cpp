#include "SacredGeometryTemplate.h"
#include "../shape/CartesianAxes.h"
#include "../shape/CircleShape.h"
#include "../shape/CrossLine.h"
#include "../shape/DotShape.h"
#include "../shape/OctagramLine.h"
#include "../shape/ParallelogramLine.h"
#include "../shape/RectangleLine.h"

//--------------------------------------------------------------
void SacredGeometryTemplate::startSequentialDrawing() {
  // Cek apakah sequential sudah pernah selesai
  if (sequentialCompleted) {
    // Sequential sudah selesai sebelumnya, jangan jalankan lagi
    return;
  }

  // Reset semua shapes ke progress 0 untuk memastikan mulai dari awal
  for (auto &shape : shapes) {
    shape->progress = 0;
  }

  // Mulai sequential mode
  sequentialMode = true;
  sequentialCompleted = false;
  currentShapeIndex = 0;

  // Set sequential mode untuk semua shapes
  for (auto &shape : shapes) {
    shape->setSequentialMode(sequentialMode);
  }

  // Apply labels dan dots visibility setting
  toggleLabels(); // Apply labelsVisible setting
  toggleDots();   // Apply dotsVisible setting

  // Apply line width setting
  updateLineWidth(lineWidth);
}

//--------------------------------------------------------------
bool SacredGeometryTemplate::updateSequentialDrawing(float deltaTime) {
  if (!sequentialMode)
    return true;

  AbstractShape *currentShape = nullptr;

  // Tentukan shape berdasarkan index
  if (currentShapeIndex >= 0 && currentShapeIndex < shapes.size()) {
    currentShape = shapes[currentShapeIndex].get();
  }

  // UPDATE current shape DULU sebelum cek complete!
  if (currentShape) {
    currentShape->update(deltaTime);
  }

  // Cek jika current shape sudah complete
  if (currentShape && currentShape->isComplete()) {
    // Pindah ke shape berikutnya
    currentShapeIndex++;

    // Cek apakah masih ada shape yang perlu di-draw
    if (currentShapeIndex < shapes.size()) {
      return false; // Belum selesai, lanjut ke shape berikutnya
    } else {
      // Semua shapes sudah complete, matikan sequential mode dan tandai selesai
      sequentialMode = false;
      sequentialCompleted = true;
      return true; // Selesai!
    }
  }

  return false; // Belum selesai
}

//--------------------------------------------------------------
void SacredGeometryTemplate::clearAllShapes() {
  // Matikan sequential mode
  sequentialMode = false;

  // Benar-benar HAPUS semua shapes dari memory
  shapes.clear();

  // Reset sequential completed flag
  sequentialCompleted = false;
}

//--------------------------------------------------------------
void SacredGeometryTemplate::drawParallel() {
  // Matikan sequential mode (paralel mode)
  sequentialMode = false;

  // Set mode untuk semua shapes (pakai sequentialMode yang sudah false)
  for (auto &shape : shapes) {
    shape->setSequentialMode(sequentialMode);
  }

  // Apply labels dan dots visibility setting
  toggleLabels(); // Apply labelsVisible setting
  toggleDots();   // Apply dotsVisible setting

  // Apply line width setting
  updateLineWidth(lineWidth);

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
  for (auto &shape : shapes) {
    shape->setLineWidth(lineWidth);
  }
}

//--------------------------------------------------------------
void SacredGeometryTemplate::updateLineWidth(float width) {
  lineWidth = width;

  // Update line width untuk semua shapes
  for (auto &shape : shapes) {
    shape->setLineWidth(lineWidth);
  }
}

//--------------------------------------------------------------
void SacredGeometryTemplate::enableAllDrawSettings() {
  // Enable semua draw settings untuk SacredGeometry window
  // SacredGeometry harus INDEPENDENT dari Playground checkbox preferences
  drawCartesian = true;
  drawCircles = true;
  drawCrossLines = true;
  drawParallelograms = true;
  drawRectangleLines = true;
  drawOctagramLines = true;
}

//--------------------------------------------------------------
void SacredGeometryTemplate::applySpeedMultiplier() {
  // Base speed untuk setiap tipe shape (sesuai kalibrasi delta time)
  // CircleShape, CrossLine, ParallelogramLine, RectangleLine, OctagramLine,
  // DotShape: 50.0f CartesianAxes: 1.2f

  for (auto &shape : shapes) {
    // Cek tipe shape dan apply base speed yang sesuai
    if (dynamic_cast<CircleShape *>(shape.get())) {
      shape->speed = 20.0f * templateSpeedMultiplier;
    } else if (dynamic_cast<CrossLine *>(shape.get())) {
      shape->speed = 20.0f * templateSpeedMultiplier;
    } else if (dynamic_cast<ParallelogramLine *>(shape.get())) {
      shape->speed = 20.0f * templateSpeedMultiplier;
    } else if (dynamic_cast<RectangleLine *>(shape.get())) {
      shape->speed = 20.0f * templateSpeedMultiplier;
    } else if (dynamic_cast<OctagramLine *>(shape.get())) {
      shape->speed = 20.0f * templateSpeedMultiplier;
    } else if (dynamic_cast<CartesianAxes *>(shape.get())) {
      shape->speed = 0.5f * templateSpeedMultiplier;
    } else if (dynamic_cast<DotShape *>(shape.get())) {
      shape->speed = 20.0f * templateSpeedMultiplier;
    } else {
      // Default fallback for any other shape type
      shape->speed = 20.0f * templateSpeedMultiplier;
    }

    // DEBUG LOG (Temporary)
    // ofLog() << "Applied Speed: " << shape->speed << " (Mult: " <<
    // templateSpeedMultiplier << ")";
  }
}

//--------------------------------------------------------------
// ⭐ NEW: Draw dengan virtual sequential progress untuk tessellation radial expansion
// virtualTime: waktu animasi dalam detik (0.0 = mulai, >0 = progress)
// shapeDuration: durasi per shape dalam detik (0 = auto dari speed)
//
// Contoh: 4 shapes dengan shapeDuration = 2 detik:
// - virtualTime 0.0s: Shape 0 mulai animate (progress 0%)
// - virtualTime 1.0s: Shape 0 di 50%, shape 1-3 belum muncul
// - virtualTime 2.0s: Shape 0 complete (100%), Shape 1 mulai animate
// - virtualTime 4.0s: Shape 1 complete, Shape 2 mulai
// - virtualTime 6.0s: Shape 2 complete, Shape 3 mulai
// - virtualTime 8.0s: Semua shapes complete
//--------------------------------------------------------------
void SacredGeometryTemplate::drawAtVirtualTime(float virtualTime, float shapeDuration) {
  if (shapes.empty()) return;

  // Save original progress values untuk restore nanti
  std::vector<float> originalProgress;
  originalProgress.reserve(shapes.size());
  for (const auto& shape : shapes) {
    originalProgress.push_back(shape->progress);
  }

  // Calculate actual shape duration jika tidak ditentukan
  float actualDuration = shapeDuration;
  if (actualDuration <= 0.0f && !shapes.empty()) {
    // Gunakan speed dari shape pertama untuk calculate duration
    float baseSpeed = shapes[0]->speed;
    if (baseSpeed > 0) {
      actualDuration = shapes[0]->getMaxProgress() / (baseSpeed * 60.0f); // 60 FPS
    } else {
      actualDuration = 2.0f; // Default 2 detik
    }
  }

  // Update dan draw shapes berdasarkan virtualTime
  for (size_t i = 0; i < shapes.size(); ++i) {
    float shapeStartTime = i * actualDuration;
    float shapeEndTime = (i + 1) * actualDuration;

    if (virtualTime >= shapeEndTime) {
      // Shape sudah complete → draw full
      shapes[i]->progress = shapes[i]->getMaxProgress();
      shapes[i]->draw();
    } else if (virtualTime >= shapeStartTime) {
      // Shape sedang animating → calculate progress
      float timeIntoShape = virtualTime - shapeStartTime;
      float progressRatio = timeIntoShape / actualDuration;
      shapes[i]->progress = progressRatio * shapes[i]->getMaxProgress();
      shapes[i]->draw();
    }
    // else: shape belum mulai → jangan draw
  }

  // Restore original progress values (jangan modify state asli!)
  for (size_t i = 0; i < shapes.size(); ++i) {
    shapes[i]->progress = originalProgress[i];
  }
}
