#pragma once
#include "AbstractShape.h"
#include "ofMain.h"
#include <string>


class CircleShape : public AbstractShape {
public:
  std::string label = "";
  CircleShape(float r, std::string label, float angle, float distance);

  // Override pure virtual methods dari AbstractShape
  void update(float deltaTime = 0.016f) override;
  void draw() override;
  void setLabel(std::string label);
  void setRadius(float r) override; // Override setRadius untuk runtime update

  // Label visibility control
  void showLabel();
  void hideLabel();

  // Dot visibility control
  void showDot();
  void hideDot();

  // Override addDotsToCache untuk menambahkan circle center dot
  void addDotsToCache(std::vector<DotInfo> &dots) override;

private:
  float radius; // Radius lingkaran
  float angle;  // Angle position (0, PI/2, PI, 3*PI/2)
  float
      distance; // Distance dari center (0 untuk Circle A, radius untuk lainnya)
  float originalRadius;      // Simpan radius awal untuk re-calculate distance
  float originalDistance;    // Simpan distance awal untuk cegah float drift
  float totalSegments = 100; // Circle-specific
  bool labelVisible = true;  // Flag untuk label visibility
  bool dotVisible = true;    // Flag untuk dot visibility
};
