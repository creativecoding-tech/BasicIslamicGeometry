#pragma once

#include "../shape/AbstractShape.h"
#include "ofMain.h"
#include <memory>
#include <vector>

/**
 * SacredGeometryTemplate - Abstract base class untuk semua geometric pattern
 * templates
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

  // ⭐ NEW: Draw with virtual sequential progress untuk tessellation
  // virtualTime: waktu animasi dalam detik (untuk radial expansion)
  // shapeDuration: durasi per shape dalam detik (default dihitung dari speed)
  // Ini menggambar seolah-olah template di-state tertentu tanpa mengubah state asli
  virtual void drawAtVirtualTime(float virtualTime, float shapeDuration = 0.0f);

  // Update logic - default implementation update semua shapes
  // Bisa di-override untuk custom update behavior
  virtual void update(float deltaTime = 0.016f) { // Default 60 FPS = 0.016f
    for (auto &shape : shapes) {
      if (shape) {
        shape->update(deltaTime);
      }
    }
  }

  // Radius management - HANYA update radius value (TANPA rebuild shapes)
  virtual void setRadius(float r) {
    radius = r;
    // Hanya simpan radius - JANGAN rebuild shapes!
  }

  // Get read-only access ke shapes (untuk file operations, dll)
  const std::vector<std::unique_ptr<AbstractShape>> &getShapes() const {
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
  bool drawCartesian = true; // Playground panel preference saat Draw

  // Circle visibility preferences (public agar bisa diakses dari UI)
  bool drawCircles = true; // Playground panel preference saat Draw
  std::vector<int>
      circleIndices; // Daftar index circles di template (fleksibel!)

  // CrossLine visibility preferences (public agar bisa diakses dari UI)
  bool drawCrossLines = true; // Playground panel preference saat Draw
  std::vector<int>
      crossLineIndices; // Daftar index crosslines di template (fleksibel!)

  // Parallelogram visibility preferences (public agar bisa diakses dari UI)
  bool drawParallelograms = true; // Playground panel preference saat Draw
  std::vector<int> parallelogramIndices; // Daftar index parallelograms di
                                         // template (fleksibel!)

  // RectangleLine visibility preferences (public agar bisa diakses dari UI)
  bool drawRectangleLines = true; // Playground panel preference saat Draw
  std::vector<int> rectangleLineIndices; // Daftar index rectangleLines di
                                         // template (fleksibel!)

  // OctagramLine visibility preferences (public agar bisa diakses dari UI)
  bool drawOctagramLines = true;        // Playground panel preference saat Draw
  std::vector<int> octagramLineIndices; // Daftar index octagramLines di
                                        // template (fleksibel!)

  // Polygon animation mode (public agar bisa diakses dari UI)
  int polygonAnimationMode = 0; // 0 = No Animation, 1 = FadeIn

  // Speed control (public agar bisa diakses dari UI)
  float speedMultiplier = 1.0f; // General/Base speed (deprecated/legacy)
  float templateSpeedMultiplier =
      1.0f; // ⭐ NEW: Specific for template dots/lines
  float customLineSpeedMultiplier = 1.0f; // ⭐ NEW: Specific for custom lines
  float polygonSpeedMultiplier = 1.0f;    // ⭐ NEW: Specific for polygons
  float specialSpeedMultiplier = 1.0f;    // ⭐ NEW: Specific for special polygon animation
  void applySpeedMultiplier(); // Apply speed multiplier ke semua shapes

  // Template control methods - fully autonomous!
  virtual void startSequentialDrawing();
  virtual bool updateSequentialDrawing(
      float deltaTime = 0.016f); // Return true jika complete
  virtual void
  clearAllShapes(); // Benar-benar hapus semua shapes (shapes.clear())
  virtual void drawParallel(); // Draw all shapes secara parallel (setup + show)
  virtual void toggleLabels();
  virtual void toggleDots();
  virtual void setLineWidth(float width);
  virtual void updateLineWidth(float width); // Realtime update tanpa rebuild
  virtual void enableAllDrawSettings(); // Enable semua draw settings (untuk
                                        // SacredGeometry window)

  // Optional: Template-specific settings
  virtual bool hasCustomSettings() { return false; }
  virtual void showSettingsUI() {}

  // Optional: Template-specific playback settings (Mode Draw, Auto Clean, Play)
  virtual bool hasPlaybackSettings() { return false; }
  virtual void showPlaybackUI(class ofApp *app) {}

protected:
  // Template sekarang OWN shapes-nya sendiri!
  // Derived classes populate ini via setupShapes()
  std::vector<std::unique_ptr<AbstractShape>> shapes;
};
