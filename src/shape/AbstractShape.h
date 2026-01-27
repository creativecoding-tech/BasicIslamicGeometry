#pragma once
#include "ofMain.h"
#include "DotInfo.h"
#include <vector>

/**
 * AbstractShape - Base class untuk semua shape geometri
 * Menyediakan common attributes dan methods untuk animasi dan visibility control
 */
class AbstractShape {
public:
	// Common attributes untuk animasi
	float progress = 0;
	float speed = 0.5f;
	bool showing = false;
	float lineWidth = 4;

	// Constructor
	AbstractShape() = default;
	virtual ~AbstractShape() = default;

	// Common methods untuk visibility control
	virtual void show();
	virtual void hide();

	// Pure virtual methods - WAJIB di-override oleh derived class
	virtual void update() = 0;
	virtual void draw() = 0;

	// Polymorphic method untuk menambahkan dots ke cache (default: no-op)
	// Derived classes yang punya dots (Circle, CrossLine, dll) harus override
	virtual void addDotsToCache(std::vector<DotInfo>& dots) {}

	// Label dan dot control (default: no-op, dapat di-override jika diperlukan)
	virtual void showLabel() {}
	virtual void hideLabel() {}
	virtual void showDot() {}
	virtual void hideDot() {}

	// Sequential mode control (default: no-op, OctagramLine akan override)
	virtual void setSequentialMode(bool sequential) {}

	// Common method untuk line width
	virtual void setLineWidth(float width);

	// Optional - dapat di-override jika diperlukan
	virtual void setThin(bool thin) { useThin = thin; }

	// Status check - dapat di-override jika logic berbeda
	virtual bool isComplete() {
		if (showing) {
			return progress >= maxProgress;
		}
		else {
			return progress <= 0;
		}
	}

protected:
	// Protected attributes untuk derived classes
	float maxProgress = 100;  // Default max progress (dapat di-override)
	bool useThin = false;     // Flag untuk font thin/bold

	// Protected methods untuk font (hanya class yang butuh font)
	ofTrueTypeFont fontNormal;
	ofTrueTypeFont fontBold;

	// Helper method untuk load font
	void loadFonts(const string& normalPath = "C:\\Windows\\Fonts\\calibri.ttf",
				   const string& boldPath = "C:\\Windows\\Fonts\\calibrib.ttf",
				   int normalSize = 15, int boldSize = 20);
};
