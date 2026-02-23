#pragma once

#include "../anim/AbstractAnimation.h"
#include "../anim/WaveLineAnimation.h"
#include "ofMain.h"
#include <memory>

using glm::vec2;

/**
 * CustomLine - Garis custom yang bisa dibuat user dengan mouse drag
 * Mendukung:
 * - Garis lurus
 * - Bezier curve dengan parameter curve
 * - Progressive drawing animation
 * - Axis lock untuk DcustomLine (duplicate lines)
 */
enum class AxisLock {
  NONE,     // Bebas bergerak di X dan Y
  LOCK_X,   // Hanya bisa bergerak di Y (atas bawah)
  LOCK_Y,   // Hanya bisa bergerak di X (kanan kiri)
  LOCK_BOTH // Tidak bisa gerak sama sekali (fully locked)
};

class CustomLine {
public:
  // Constructor
  CustomLine();
  CustomLine(vector<vec2> points, ofColor color, float lineWidth,
             std::string label = "");

  // Copy assignment operator (animation tidak dicopy) ⭐ NEW
  CustomLine &operator=(const CustomLine &other);

  // Main drawing method
  void draw() const;

  // Setters
  void setPoints(const vector<vec2> &points);
  void setColor(ofColor color);
  void setLineWidth(float width);
  void setCurve(float curve);
  void setProgress(float progress);
  void setSpeed(float speed);
  void setLabel(const std::string &label);

  // Getters
  const vector<vec2> &getPoints() const { return points; }
  vector<vec2> getSampledPoints()
      const; // Ambil semua titik sampling dari curve (100 segments)
  ofColor getColor() const { return color; }
  float getLineWidth() const { return lineWidth; }
  float getCurve() const { return curve; }
  float getProgress() const { return progress; }
  float getSpeed() const { return speed; }
  const std::string &getLabel() const { return label; }

  // Selection state
  void setSelected(bool selected);
  bool isSelected() const { return selected; }

  // Duplicate state
  void setIsDuplicate(bool duplicate) { isDuplicate = duplicate; }
  bool getIsDuplicate() const { return isDuplicate; }

  // Axis lock state (untuk DcustomLine)
  void setAxisLock(AxisLock lock) { axisLock = lock; }
  AxisLock getAxisLock() const { return axisLock; }

  // Animation update
  void updateProgress(float deltaTime = 0.016f);
  void update(float deltaTime = 0.016f); // Update animation (wave) ⭐ NEW
  bool hasAnimation() const;             // Cek apakah punya animation ⭐ NEW
  std::shared_ptr<AbstractAnimation>
  getAnimation() const; // Getter untuk animation ⭐ NEW
  bool hasFinishedAnimation() const { return isAnimationFinished; } // ⭐ NEW
  void
  setAnimation(std::shared_ptr<AbstractAnimation> anim,
               float duration = 5.0f); // Set animation dengan durasi ⭐ CHANGED

  // Load state ⭐ NEW
  void setLoadedFromFile(bool loaded) { loadedFromFile = loaded; }
  bool isLoadedFromFile() const { return loadedFromFile; }

  // Tessellated state ⭐ NEW
  void setTessellated(bool tess) { tessellated = tess; }
  bool isTessellated() const { return tessellated; }

  // Helper for curve tracking
  vec2 getPointAt(float t) const;      // Get point at t (0.0 - 1.0)
  float getClosestT(vec2 point) const; // Get closest t for a point
  float getApproxLength() const; // Get approximate length of the line/curve

public:
  // Method untuk set titik asli dari luar (dipanggil setelah instantiate) ⭐
  // NEW
  void saveOriginalPoints(float currentTemplateRadius);
  // Method untuk scale geometri berdasar ratio absolute dari original ⭐ NEW
  void scaleToRadius(float newRadius);

private:
  vector<vec2> originalPoints; // Backup dari titik asli untuk scaling tanpa
                               // cacat presisi ⭐ NEW
  float originalCurve;         // Backup dari curve asli
  float baseRadius =
      1.0f; // Radius template saat titik asli dibuat/diload ⭐ NEW

  vector<vec2> points; // 2 titik: start dan end
  ofColor color;
  float lineWidth;
  float curve;    // 0 = lurus, != 0 = bezier curve
  float progress; // 0.0 - 1.0 untuk progressive drawing
  float speed;    // Kecepatan animasi
  bool selected;  // Selection state untuk visual feedback
  std::string
      label; // Label untuk customLine (cth: "customLine0", "DcustomLine0")
  bool isDuplicate;  // True jika ini duplicate line (hasil dari
                     // duplicateLineR180)
  AxisLock axisLock; // Axis lock untuk DcustomLine (NONE, LOCK_X, LOCK_Y)
  std::shared_ptr<AbstractAnimation>
      animation; // Animation system (WaveLineAnimation, dll) ⭐ CHANGED TO
                 // shared_ptr
  bool loadedFromFile = false; // Flag: true jika diload dari file .nay
  bool tessellated = false;    // Flag: true jika ini hasil tessellasi
  float animationTimer = 0.0f; // Timer untuk tracking durasi animasi (generik,
                               // bukan cuma wave) ⭐ NEW
  float animationAutoStopDuration =
      5.0f; // Durasi auto-stop untuk animasi (0 = infinite) ⭐ NEW
  bool animationTimerInitialized =
      false; // Flag untuk mencegah double-reset timer ⭐ NEW
  bool isAnimationFinished =
      false; // Flag penanda animasi sudah selesai durasinya ⭐ NEW

  // Helper methods
  void drawStraightLine(vec2 start, vec2 end) const;
  void drawCurvedLine(vec2 start, vec2 end) const;
  vec2 calculateControlPoint(vec2 start, vec2 end) const;
};
