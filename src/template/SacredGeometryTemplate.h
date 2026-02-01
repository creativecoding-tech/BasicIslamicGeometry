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
	virtual void update() {
		for (auto &shape : shapes) {
			if (shape) {
				shape->update();
			}
		}
	}

	// Radius management - update radius dan rebuild shapes
	virtual void setRadius(float r) {
		radius = r;
		// Clear dan rebuild shapes dengan radius baru
		shapes.clear();
		setupShapes();
	}

	// Get read-only access ke shapes (untuk file operations, dll)
	const std::vector<std::unique_ptr<AbstractShape>>& getShapes() const {
		return shapes;
	}

	// Public radius - bisa diakses langsung
	float radius = 240;

	// Optional: Template-specific settings
	virtual bool hasCustomSettings() { return false; }
	virtual void showSettingsUI() {}

protected:
	// Template sekarang OWN shapes-nya sendiri!
	// Derived classes populate ini via setupShapes()
	std::vector<std::unique_ptr<AbstractShape>> shapes;
};
