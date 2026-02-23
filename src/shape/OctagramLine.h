#pragma once
#include "AbstractShape.h"
#include "ofMain.h"
#include <optional>
#include <string>

// namespace alias
using glm::vec2;
using std::string;
class OctagramLine : public AbstractShape {
public:
  vec2 start;
  vec2 end;
  std::optional<vec2>
      nextPoint; // Optional titik lanjutan (jika ada, gambar 2 garis)
  std::string label = "";
  OctagramLine(vec2 start, vec2 end, std::optional<vec2> nextPoint,
               string label, float radius);
  // Helper method position
  void setStart(float startX, float startY);
  void setEnd(float endX, float endY);

  // Dot visibility control
  void showDot();
  void hideDot();

  // Override addDotsToCache untuk menambahkan Octagram dots
  void addDotsToCache(std::vector<DotInfo> &dots) override;

  // Label visibility control
  void showLabel();
  void hideLabel();

  // Override setRadius untuk runtime update
  void setRadius(float r) override;

  // Override pure virtual methods dari AbstractShape
  void update(float deltaTime = 0.016f) override;
  void draw() override;

  // Override isComplete untuk ngecek kedua progress (main + extension)
  bool isComplete() override;

  // Override setSequentialMode untuk kontrol animasi
  void setSequentialMode(bool sequential) override {
    isSequentialMode = sequential;
  }

private:
  vec2 originalStart;
  vec2 originalEnd;
  std::optional<vec2> originalNextPoint;
  float radius;                // Radius template
  float originalRadius;        // Simpan radius awal untuk proportional scaling
  float totalSegments = 100;   // specific
  float extensionProgress = 0; // Progress untuk extension line (phase 2)
  bool isSequentialMode = false; // Flag untuk sequential vs paralel mode
  bool dotVisible = true;        // Flag untuk dot visibility
  bool labelVisible = true;      // Flag untuk label visibility
};
