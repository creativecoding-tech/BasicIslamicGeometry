#pragma once

#include "ofMain.h"
#include <vector>
#include <memory>
#include "../shape/AbstractShape.h"

/**
 * SacredGeometryTemplate - Abstract base class untuk semua geometric pattern templates
 *
 * Setiap template (Zellige, Flower of Life, dll) harus inherit dari class ini
 * dan implement setupShapes() untuk mendefinisikan pattern-nya.
 *
 * Template sekarang SELF-CONTAINED:
 * - Owns shapes sendiri (protected member)
 * - Handle drawing sendiri (draw())
 * - Handle update sendiri (update())
 * - Handle radius changes sendiri (setRadius())
 *
 * Usage:
 *   class MyTemplate : public SacredGeometryTemplate {
 *       void setupShapes() override {
 *           shapes.push_back(std::make_unique<CircleShape>(240, "A", 0, 0));
 *           // ... tambah shapes lainnya
 *       }
 *   };
 *
 * Di ofApp:
 *   currentTemplate->draw();
 *   currentTemplate->update();
 *   currentTemplate->setRadius(300);
 */
class SacredGeometryTemplate {
public:
	virtual ~SacredGeometryTemplate() = default;

	// Get nama template (untuk display di UI)
	virtual std::string getName() = 0;

	// Get deskripsi singkat template
	virtual std::string getDescription() = 0;

	// Setup semua shapes yang diperlukan template ini
	// Template sekarang populate ke internal 'shapes' vector
	virtual void setupShapes() = 0;

	// Drawing logic - default implementation loop semua shapes
	// Bisa di-override untuk custom rendering behavior
	virtual void draw() {
		for (auto &shape : shapes) {
			if (shape) {
				shape->draw();
			}
		}
	}

	// Update logic - default implementation update semua shapes
	// Bisa di-override untuk custom update behavior
	virtual void update(float deltaTime = 0.016f) {  // Default 60 FPS = 0.016f
		for (auto &shape : shapes) {
			if (shape) {
				shape->update(deltaTime);
			}
		}
	}

	// Radius management - update radius dan rebuild shapes
	virtual void setRadius(float r) {
		radius = r;
		// Clear dan rebuild shapes dengan radius baru
		shapes.clear();
		setupShapes();
		applySpeedMultiplier();  // Re-apply speed multiplier setelah rebuild!
	}

	// Get read-only access ke shapes (untuk file operations, dll)
	const std::vector<std::unique_ptr<AbstractShape>>& getShapes() const {
		return shapes;
	}

	// Public radius - bisa diakses langsung
	float radius = 214;

	// Template state
	bool sequentialMode = false;
	int currentShapeIndex = 0;
	bool sequentialCompleted = false;
	bool labelsVisible = true;
	bool dotsVisible = true;
	float lineWidth = 4.0f;

	// Cartesian visibility preferences (public agar bisa diakses dari UI)
	bool showCartesianOnPlay = true;  // Playground panel preference saat Play
	bool showCartesianInSacredGeometry = true;  // SacredGeometry panel preference (realtime)

	// Circle visibility preferences (public agar bisa diakses dari UI)
	bool showCirclesOnPlay = true;  // Playground panel preference saat Play
	std::vector<int> circleIndices;  // Daftar index circles di template (fleksibel!)

	// CrossLine visibility preferences (public agar bisa diakses dari UI)
	bool showCrossLinesOnPlay = true;  // Playground panel preference saat Play
	std::vector<int> crossLineIndices;  // Daftar index crosslines di template (fleksibel!)

	// Parallelogram visibility preferences (public agar bisa diakses dari UI)
	bool showParallelogramsOnPlay = true;  // Playground panel preference saat Play
	std::vector<int> parallelogramIndices;  // Daftar index parallelograms di template (fleksibel!)

	// RectangleLine visibility preferences (public agar bisa diakses dari UI)
	bool showRectangleLinesOnPlay = true;  // Playground panel preference saat Play
	std::vector<int> rectangleLineIndices;  // Daftar index rectangleLines di template (fleksibel!)

	// OctagramLine visibility preferences (public agar bisa diakses dari UI)
	bool showOctagramLinesOnPlay = true;  // Playground panel preference saat Play
	std::vector<int> octagramLineIndices;  // Daftar index octagramLines di template (fleksibel!)

	// Polygon animation mode (public agar bisa diakses dari UI)
	int polygonAnimationMode = 0;  // 0 = No Animation, 1 = FadeIn

	// Speed control (public agar bisa diakses dari UI)
	float speedMultiplier = 1.0f;  // 1.0 = normal speed, 0.5 = half speed, 2.0 = double speed
	void applySpeedMultiplier();  // Apply speed multiplier ke semua shapes

	// Template control methods - fully autonomous!
	virtual void startSequentialDrawing();
	virtual bool updateSequentialDrawing();  // Return true jika complete
	virtual void showAllShapes();
	virtual void hideAllShapes();
	virtual void toggleLabels();
	virtual void toggleDots();
	virtual void setLineWidth(float width);
	virtual void updateLineWidth(float width);  // Realtime update tanpa rebuild

	// Optional: Template-specific settings
	virtual bool hasCustomSettings() { return false; }
	virtual void showSettingsUI() {}

	// Optional: Template-specific playback settings (Mode Draw, Auto Clean, Play)
	virtual bool hasPlaybackSettings() { return false; }
	virtual void showPlaybackUI(class ofApp* app) {}

protected:
	// Template sekarang OWN shapes-nya sendiri!
	// Derived classes populate ini via setupShapes()
	std::vector<std::unique_ptr<AbstractShape>> shapes;
};
