#include "CrossLine.h"
#include "DotInfo.h"
#include <ofMain.h>

CrossLine::CrossLine(vec2 start, vec2 end, string label1, string label2,
                     float radius)
    : start(start), end(end), originalStart(start), originalEnd(end),
      label1(label1), label2(label2), radius(radius),
      originalRadius(radius), // Simpan original radius
      radiusDot(vec2(0, 0)), originalRadiusDot(vec2(0, 0)) {
  // SIMPAN kuadran dari end position
  if (end.x >= 0 && end.y >= 0)
    quadrant = 0; // (+,+) Kuadran 1 (45°)
  else if (end.x < 0 && end.y >= 0)
    quadrant = 1; // (-,+) Kuadran 2 (135°)
  else if (end.x < 0 && end.y < 0)
    quadrant = 2; // (-,-) Kuadran 3 (-135°)
  else
    quadrant = 3; // (+,-) Kuadran 4 (-45°)

  // Hitung radiusDot SEKALI di constructor untuk akses dari ofApp
  float totalAngle = atan2(end.y - start.y, end.x - start.x);
  radiusDot = vec2(cos(totalAngle) * radius, sin(totalAngle) * radius);
  originalRadiusDot = radiusDot;

  loadFonts();                 // Load font dari AbstractShape
  maxProgress = totalSegments; // Set max progress untuk isComplete()
}

void CrossLine::setLabel1(string lbl) { label1 = lbl; }

void CrossLine::setLabel2(string lbl) { label2 = lbl; }

void CrossLine::setStart(float startX, float startY) {
  start = vec2(startX, startY);
  originalStart = start;
  // Re-calculate radiusDot agar sync dengan posisi baru
  float totalAngle = atan2(end.y - start.y, end.x - start.x);
  radiusDot = vec2(cos(totalAngle) * radius, sin(totalAngle) * radius);
  originalRadiusDot = radiusDot;
}

void CrossLine::setEnd(float endX, float endY) {
  end = vec2(endX, endY);
  originalEnd = end;
  // Re-calculate radiusDot agar sync dengan posisi baru
  float totalAngle = atan2(end.y - start.y, end.x - start.x);
  radiusDot = vec2(cos(totalAngle) * radius, sin(totalAngle) * radius);
  originalRadiusDot = radiusDot;
}

void CrossLine::setRadius(float r) {
  if (originalRadius <= 0.0f)
    return;
  // Re-calculate secara proporsional dari original
  float scaleFactor = r / originalRadius;

  if (std::abs(scaleFactor - 1.0f) < 0.0001f) {
    start = originalStart;
    end = originalEnd;
    radiusDot = originalRadiusDot;
  } else {
    start = originalStart * scaleFactor;
    end = originalEnd * scaleFactor;
    radiusDot = originalRadiusDot * scaleFactor;
  }

  radius = r;
  // DO NOT update originalRadius to prevent drift
}

void CrossLine::showLabel() { labelVisible = true; }

void CrossLine::hideLabel() { labelVisible = false; }

void CrossLine::showDot() { dotVisible = true; }

void CrossLine::hideDot() { dotVisible = false; }

void CrossLine::update(float deltaTime) {
  // Animasi muncul dari 0 ke totalSegments
  if (progress < totalSegments)
    progress += speed * deltaTime;
}

void CrossLine::draw() {
  ofPushMatrix();
  ofNoFill();
  ofSetColor(0);
  ofSetLineWidth(lineWidth);

  // Polar Thinking untuk line drawing
  // Hitung angle dan distance dari start ke end
  float totalAngle = atan2(end.y - start.y, end.x - start.x);
  float totalDistance = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));

  ofPolyline polyline;
  // Gambar garis secara progressif menggunakan polar coordinates
  for (int i = 0; i <= progress; i++) {
    float t = ofMap(i, 0, totalSegments, 0, 1);
    // Interpolate distance sepanjang angle yang sama
    float currentDist = totalDistance * t;
    // Konversi polar ke cartesian: x = start + cos(angle) * distance
    float x = start.x + cos(totalAngle) * currentDist;
    float y = start.y + sin(totalAngle) * currentDist;

    polyline.addVertex(x, y);
  }
  polyline.close();
  polyline.draw();

  if (progress >= totalSegments) {
    ofFill();
    // Gambar dot hanya jika dotVisible = true
    if (dotVisible) {
      ofDrawCircle(radiusDot.x, radiusDot.y, lineWidth * 2);
      // Dot or circle end x and y
      ofDrawCircle(end.x, end.y, lineWidth * 2);
    }

    ofSetColor(0);

    if (labelVisible) {
      fontNormal.drawString(label1, radiusDot.x, radiusDot.y - 10);
      fontNormal.drawString(label2, end.x - 20, end.y - 5);
    }
  }
  ofPopMatrix();
}

//--------------------------------------------------------------
void CrossLine::addDotsToCache(std::vector<DotInfo> &dots) {
  // CrossLine punya 2 dots: radiusDot dan end
  dots.push_back({radiusDot, "CrossLine"});
  dots.push_back({end, "CrossLine"});
}
