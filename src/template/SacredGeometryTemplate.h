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
 * Usage:
 *   class MyTemplate : public SacredGeometryTemplate {
 *       void setupShapes(std::vector<std::unique_ptr<AbstractShape>>& shapes) override {
 *           shapes.push_back(std::make_unique<CircleShape>(240, "A", 0, 0));
 *           // ... tambah shapes lainnya
 *       }
 *   };
 */
class SacredGeometryTemplate {
public:
	virtual ~SacredGeometryTemplate() = default;

	// Get nama template (untuk display di UI)
	virtual std::string getName() = 0;

	// Get deskripsi singkat template
	virtual std::string getDescription() = 0;

	// Setup semua shapes yang diperlukan template ini
	// Shapes akan disimpan di vector yang di-pass by reference
	virtual void setupShapes(std::vector<std::unique_ptr<AbstractShape>>& shapes) = 0;

	// Get default radius untuk template ini
	virtual float getDefaultRadius() = 0;

	// Optional: Template-specific settings
	virtual bool hasCustomSettings() { return false; }
	virtual void showSettingsUI() {}
};
