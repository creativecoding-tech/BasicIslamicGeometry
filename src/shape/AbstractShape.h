#pragma once
#include "DotInfo.h"
#include "ofMain.h"
#include <vector>

/**
 * AbstractShape - Base class untuk semua shape geometri
 * Menyediakan common attributes dan methods untuk animasi dan visibility
 * control
 */
class AbstractShape {
public:
  // Common attributes untuk animasi
  float progress = 0;
  float speed = 20.0f; // Delta time calibrated (60 FPS * 0.5f lama)
  float lineWidth = 4;

  // Constructor
  AbstractShape() = default;
  virtual ~AbstractShape() = default;

  // Pure virtual methods - WAJIB di-override oleh derived class
  virtual void update(float deltaTime = 0.016f) = 0; // Default 60 FPS
  virtual void draw() = 0;

  // Polymorphic method untuk menambahkan dots ke cache (default: no-op)
  // Derived classes yang punya dots (Circle, CrossLine, dll) harus override
  virtual void addDotsToCache(std::vector<DotInfo> &dots) {}

  // Label dan dot control (default: no-op, dapat di-override jika diperlukan)
  virtual void showLabel() {}
  virtual void hideLabel() {}
  virtual void showDot() {}
  virtual void hideDot() {}

  // Sequential mode control (default: no-op, OctagramLine akan override)
  virtual void setSequentialMode(bool sequential) {}

  // Common method untuk line width
  virtual void setLineWidth(float width);

  // Runtime radius update (default: no-op, shapes dengan radius akan override)
  virtual void setRadius(float r) {}

  // Set Animation Speed
  virtual void setSpeed(float speed) { this->speed = speed; }

  // Optional - dapat di-override jika diperlukan
  virtual void setThin(bool thin) { useThin = thin; }

  // Status check - dapat di-override jika logic berbeda
  virtual bool isComplete() { return progress >= maxProgress; }

  // Getter untuk maxProgress (untuk tessellation virtual time drawing)
  float getMaxProgress() const { return maxProgress; }

protected:
  // Protected attributes untuk derived classes
  float maxProgress = 100; // Default max progress (dapat di-override)
  bool useThin = false;    // Flag untuk font thin/bold

  // Protected methods untuk font (hanya class yang butuh font)
  ofTrueTypeFont fontNormal;
  ofTrueTypeFont fontBold;

  // Helper method untuk load font
  void loadFonts(const string &normalPath = "C:\\Windows\\Fonts\\calibri.ttf",
                 const string &boldPath = "C:\\Windows\\Fonts\\calibrib.ttf",
                 int normalSize = 15, int boldSize = 20);
};
