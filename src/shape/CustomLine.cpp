#include "CustomLine.h"
#include "../anim/WaveLineAnimation.h"

//--------------------------------------------------------------
CustomLine::CustomLine()
    : lineWidth(3.0f), curve(0.0f), progress(1.0f),
      speed(1.2f) // Delta time calibrated (0.02f * 60 FPS)
      ,
      selected(false), color(ofColor(0, 0, 255)) // Default biru
      ,
      label(""), isDuplicate(false), axisLock(AxisLock::NONE),
      animation(nullptr), loadedFromFile(false), tessellated(false),
      animationTimer(0.0f), animationAutoStopDuration(5.0f),
      animationTimerInitialized(false) {}

//--------------------------------------------------------------
CustomLine::CustomLine(vector<vec2> points, ofColor color, float lineWidth,
                       std::string label)
    : points(points), color(color), lineWidth(lineWidth), curve(0.0f),
      progress(1.0f), speed(1.2f) // Delta time calibrated (0.02f * 60 FPS)
      ,
      selected(false), label(label), isDuplicate(false),
      axisLock(AxisLock::NONE), animation(nullptr), loadedFromFile(false),
      tessellated(false), animationTimer(0.0f), animationAutoStopDuration(5.0f),
      animationTimerInitialized(false) {}

//--------------------------------------------------------------
CustomLine &CustomLine::operator=(const CustomLine &other) {
  if (this != &other) {
    points = other.points;
    color = other.color;
    lineWidth = other.lineWidth;
    curve = other.curve;
    progress = other.progress;
    speed = other.speed;
    selected = other.selected;
    label = other.label;
    isDuplicate = other.isDuplicate;
    axisLock = other.axisLock;
    loadedFromFile = other.loadedFromFile;
    tessellated = other.tessellated;
    animation = nullptr;   // Animation tidak dicopy (reset ke nullptr) ⭐ NEW
    animationTimer = 0.0f; // Reset timer ⭐ NEW
    animationAutoStopDuration = 5.0f;  // Default durasi ⭐ NEW
    animationTimerInitialized = false; // Reset flag ⭐ NEW
    isAnimationFinished = false;       // Reset flag ⭐ NEW
  }
  return *this;
}

//--------------------------------------------------------------
void CustomLine::draw() const {
  if (points.size() < 2)
    return;

  vec2 start = points[0];
  vec2 end = points[1];

  // Set warna: selalu pakai original color (tidak berubah saat selected)
  ofSetColor(color);

  ofSetLineWidth(lineWidth);

  // Gambar lurus atau curved
  if (curve != 0.0f) {
    drawCurvedLine(start, end);
  } else {
    drawStraightLine(start, end);
  }
}

//--------------------------------------------------------------
void CustomLine::drawStraightLine(vec2 start, vec2 end) const {
  // Gambar lurus dengan polar coordinates (progressive)
  float totalAngle = atan2(end.y - start.y, end.x - start.x);
  float totalDistance = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));

  // Hitung direction vector untuk wave offset ⭐ NEW
  vec2 direction = end - start;
  float dirLength = glm::length(direction);
  vec2 normalizedDir = dirLength > 0 ? direction / dirLength : vec2(0, 0);

  float totalSegments = 100.0f;
  ofPolyline polyline;
  int segmentsToDraw = static_cast<int>(totalSegments * progress);

  for (int j = 0; j <= segmentsToDraw; j++) {
    float t = ofMap(j, 0, totalSegments, 0, 1);
    float currentDist = totalDistance * t;

    float x = start.x + cos(totalAngle) * currentDist;
    float y = start.y + sin(totalAngle) * currentDist;

    // Apply wave offset jika ada WaveLineAnimation DAN drawing sudah selesai
    // (progress >= 1.0) ⭐ NEW
    vec2 pos = vec2(x, y);
    if (animation && progress >= 1.0f) {
      // Cek apakah WaveLineAnimation (pakai const_cast karena draw() const
      // method)
      if (auto *waveAnim = dynamic_cast<WaveLineAnimation *>(animation.get())) {
        vec2 waveOffset = waveAnim->getWaveOffset(t, normalizedDir);
        pos += waveOffset;
      }
    }

    polyline.addVertex(pos.x, pos.y);
  }

  if (!polyline.getVertices().empty()) {
    polyline.draw();
  }
}

//--------------------------------------------------------------
void CustomLine::drawCurvedLine(vec2 start, vec2 end) const {
  // Gambar bezier curve dengan quadratic bezier
  vec2 controlPoint = calculateControlPoint(start, end);

  // Hitung direction vector untuk wave offset ⭐ NEW
  vec2 direction = end - start;
  float dirLength = glm::length(direction);
  vec2 normalizedDir = dirLength > 0 ? direction / dirLength : vec2(0, 0);

  ofPolyline bezierPoly;
  int segments = 100;
  int segmentsToDraw = static_cast<int>(segments * progress);

  for (int j = 0; j <= segmentsToDraw; j++) {
    float t = (float)j / segments;
    // Quadratic bezier formula: (1-t)²p0 + 2(1-t)t*p1 + t²p2
    // untuk curva yang smooth
    vec2 point = start * (1 - t) * (1 - t) + controlPoint * 2 * (1 - t) * t +
                 end * t * t;

    // Apply wave offset jika ada WaveLineAnimation DAN drawing sudah selesai
    // (progress >= 1.0) ⭐ NEW
    vec2 pos = point;
    if (animation && progress >= 1.0f) {
      // Cek apakah WaveLineAnimation (pakai const_cast karena draw() const
      // method)
      if (auto *waveAnim = dynamic_cast<WaveLineAnimation *>(animation.get())) {
        vec2 waveOffset = waveAnim->getWaveOffset(t, normalizedDir);
        pos += waveOffset;
      }
    }

    bezierPoly.addVertex(pos.x, pos.y);
  }

  if (!bezierPoly.getVertices().empty()) {
    bezierPoly.draw();
  }
}

//--------------------------------------------------------------
vec2 CustomLine::calculateControlPoint(vec2 start, vec2 end) const {
  // Hitung control point untuk quadratic bezier
  vec2 midPoint = (start + end) / 2.0f;
  vec2 dir = end - start;

  // Perpendicular vector (-y, x)
  vec2 perp = vec2(-dir.y, dir.x);

  // Normalize perpendicular
  float perpLen = glm::length(perp);
  if (perpLen > 0) {
    perp = perp / perpLen;
  }

  // Control point = midpoint + curve * perpendicular
  return midPoint + perp * curve;
}

//--------------------------------------------------------------
void CustomLine::setPoints(const vector<vec2> &points) {
  this->points = points;
}

//--------------------------------------------------------------
void CustomLine::setColor(ofColor color) { this->color = color; }

//--------------------------------------------------------------
void CustomLine::setLineWidth(float width) { this->lineWidth = width; }

//--------------------------------------------------------------
void CustomLine::setCurve(float curve) { this->curve = curve; }

//--------------------------------------------------------------
void CustomLine::setProgress(float progress) { this->progress = progress; }

//--------------------------------------------------------------
void CustomLine::setSpeed(float speed) { this->speed = speed; }

//--------------------------------------------------------------
void CustomLine::setAnimation(std::shared_ptr<AbstractAnimation> anim,
                              float duration) {
  animation = anim;
  animationTimer = 0.0f; // Reset timer setiap kali animation di-set ⭐ NEW
  animationTimerInitialized =
      false; // Reset flag agar bisa di-reset lagi di update() ⭐ NEW
  isAnimationFinished = false; // Reset flag finished ⭐ NEW

  // Simpan durasi auto-stop untuk animasi (dipakai di update()) ⭐ NEW
  animationAutoStopDuration = duration;
}

//--------------------------------------------------------------
void CustomLine::setLabel(const std::string &label) { this->label = label; }

//--------------------------------------------------------------
void CustomLine::setSelected(bool selected) { this->selected = selected; }

//--------------------------------------------------------------
void CustomLine::updateProgress(float deltaTime) {
  if (progress < 1.0f) {
    progress += speed * deltaTime;
    if (progress > 1.0f) {
      progress = 1.0f;
    }
  }
}

//--------------------------------------------------------------
void CustomLine::update(float deltaTime) {
  // Simpan progress sebelum update untuk deteksi transisi 0.99 -> 1.0
  float prevProgress = progress;

  // Update drawing animation (progressive drawing)
  updateProgress(deltaTime);

  // Deteksi ketika progressive drawing BARU saja selesai (transisi dari < 1.0
  // ke >= 1.0)
  bool justFinishedDrawing = (prevProgress < 1.0f && progress >= 1.0f);

  // Update animation HANYA setelah progressive drawing selesai (progress
  // >= 1.0) ⭐ NEW Ini berlaku untuk SEMUA custom lines (baik dari file .nay
  // maupun manual)
  if (animation && progress >= 1.0f) {
    // Reset timer untuk 2 kondisi:
    // 1. BARU saja selesai progressive drawing
    // 2. Animation BARU saja di-set dan progress sudah >= 1.0f (dari file .nay)
    if (!animationTimerInitialized) {
      animationTimer = 0.0f;
      animationTimerInitialized = true; // Tandai bahwa sudah di-reset
    }

    // Update animation
    animation->update(deltaTime);

    // Update timer untuk auto-stop setelah durasi tercapai (jika duration > 0)
    // Jika duration <= 0, anggap infinite (tidak ada auto-stop internal)
    if (animationAutoStopDuration > 0.0f) {
      animationTimer += deltaTime;
      if (animationTimer >= animationAutoStopDuration) {
        // Durasi selesai, hapus animation (kembali ke bentuk lurus)
        animation = nullptr;
        animationTimer = 0.0f;             // Reset timer
        animationTimerInitialized = false; // Reset flag
        isAnimationFinished = true;        // Tandai selesai ⭐ NEW
      }
    }
  } else {
    // Reset flag jika tidak ada animation atau progress belum selesai
    if (!animation) {
      animationTimerInitialized = false;
    }
  }
}

//--------------------------------------------------------------
bool CustomLine::hasAnimation() const { return animation != nullptr; }

//--------------------------------------------------------------
std::shared_ptr<AbstractAnimation> CustomLine::getAnimation() const {
  return animation;
}

//--------------------------------------------------------------
vector<vec2> CustomLine::getSampledPoints() const {
  if (points.size() < 2)
    return vector<vec2>();

  vec2 start = points[0];
  vec2 end = points[1];

  // Kalau curve = 0, return 2 titik saja (lurus)
  if (curve == 0.0f) {
    return {start, end};
  }

  // Kalau curve != 0, ambil 100 sampled points dari bezier curve
  vector<vec2> sampledPoints;
  vec2 controlPoint = calculateControlPoint(start, end);
  int segments = 100;

  for (int j = 0; j <= segments; j++) {
    float t = (float)j / segments;
    vec2 point = start * (1 - t) * (1 - t) + controlPoint * 2 * (1 - t) * t +
                 end * t * t;
    sampledPoints.push_back(point);
  }

  return sampledPoints;
}

//--------------------------------------------------------------
vec2 CustomLine::getPointAt(float t) const {
  if (points.size() < 2)
    return vec2(0, 0);

  vec2 start = points[0];
  vec2 end = points[1];

  // Clamp t
  t = ofClamp(t, 0.0f, 1.0f);

  // Jika curve mendekati 0, interpolasi linear
  if (abs(curve) < 0.001f) {
    return start + (end - start) * t;
  }

  // Jika curved, gunakan quadratic bezier formula
  vec2 controlPoint = calculateControlPoint(start, end);

  // Quadratic bezier: (1-t)^2 * P0 + 2(1-t)t * P1 + t^2 * P2
  float u = 1 - t;
  return start * u * u + controlPoint * 2 * u * t + end * t * t;
}

//--------------------------------------------------------------
float CustomLine::getClosestT(vec2 point) const {
  // Brute force approximation (cukup akurat untuk UI interaction)
  // Kita sample curve di banyak titik dan cari yang paling dekat
  float closestT = 0.0f;
  float minDstSq = FLT_MAX;
  int segments = 100;

  for (int i = 0; i <= segments; i++) {
    float t = (float)i / segments;
    vec2 p = getPointAt(t);
    float dstSq = glm::distance2(p, point); // distance squared lebih cepat
    if (dstSq < minDstSq) {
      minDstSq = dstSq;
      closestT = t;
    }
  }
  return closestT;
}

//--------------------------------------------------------------
float CustomLine::getApproxLength() const {
  // Simple approximation menggunakan chord length summation
  int segments = 20;
  float length = 0.0f;
  vec2 prev = getPointAt(0.0f);

  for (int i = 1; i <= segments; i++) {
    float t = (float)i / segments;
    vec2 next = getPointAt(t);
    length += glm::distance(prev, next);
    prev = next;
  }
  return length;
}

//--------------------------------------------------------------
void CustomLine::saveOriginalPoints(float currentTemplateRadius) {
  originalPoints = points;
  originalCurve = curve;
  baseRadius = currentTemplateRadius;
}

//--------------------------------------------------------------
void CustomLine::scaleToRadius(float newRadius) {
  if (baseRadius <= 0.0f || originalPoints.empty())
    return;
  float scaleRatio = newRadius / baseRadius;

  if (std::abs(scaleRatio - 1.0f) < 0.0001f) {
    // Tidak ada scaling yang signifikan, restore ke original
    points = originalPoints;
    curve = originalCurve;
  } else {
    // Scale points
    points.resize(originalPoints.size());
    for (size_t i = 0; i < originalPoints.size(); ++i) {
      points[i] = originalPoints[i] * scaleRatio;
    }
    // Scale curve juga
    curve = originalCurve * scaleRatio;
  }
}
