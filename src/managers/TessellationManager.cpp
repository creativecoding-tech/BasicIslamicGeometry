#include "TessellationManager.h"

//--------------------------------------------------------------
TessellationManager::TessellationManager() {
  origin = vec2(0, 0);
  isActive = false;
  tessellationRadius = 120.0f;
}

//--------------------------------------------------------------
void TessellationManager::generateGrid(float radius, const vec2& viewportSize,
                                       const vec2& canvasTranslation,
                                       float canvasRotation,
                                       float canvasZoom) {
  tessellationRadius = radius;

  // Clear existing grid
  grid.clear();

  // Hitung tile size (square grid)
  float tileSize = radius * 2.0f;

  // ⭐ FIX: Hitung viewport bounds dalam WORLD space (setelah inverse canvas transform)
  // Screen space (viewport) → World space dengan inverse transform
  // Canvas transform order di ofApp::draw():
  //   1. Translate to center (w/2, h/2)
  //   2. Rotate (rotation)
  //   3. Scale (zoom)
  //   4. Pan (translation)
  //
  // Inverse transform order (REVERSE order):
  //   1. Inverse pan: subtract translation
  //   2. Inverse scale: divide by zoom
  //   3. Inverse rotate: rotate by -rotation
  //   4. Inverse center: subtract half viewport size

  float halfWidth = viewportSize.x / 2.0f;
  float halfHeight = viewportSize.y / 2.0f;

  // Transform viewport corners dari screen space ke world space
  // Top-left viewport (0, 0) → world space
  vec2 topLeft = vec2(0, 0);
  topLeft.x -= canvasTranslation.x;                      // Inverse pan: subtract
  topLeft.y -= canvasTranslation.y;
  topLeft.x /= canvasZoom;                               // Inverse scale: divide
  topLeft.y /= canvasZoom;
  topLeft = rotatePoint(topLeft, -canvasRotation);       // Inverse rotate: rotate by -angle
  topLeft.x -= halfWidth;                                // Inverse center: subtract half size
  topLeft.y -= halfHeight;

  // Bottom-right viewport (width, height) → world space
  vec2 bottomRight = vec2(viewportSize.x, viewportSize.y);
  bottomRight.x -= canvasTranslation.x;
  bottomRight.y -= canvasTranslation.y;
  bottomRight.x /= canvasZoom;
  bottomRight.y /= canvasZoom;
  bottomRight = rotatePoint(bottomRight, -canvasRotation);
  bottomRight.x -= halfWidth;
  bottomRight.y -= halfHeight;

  // Generate grid dari world space bounds
  // ⭐ FIX: StartCol/Row dihitung dari world bounds
  int startCol = static_cast<int>(std::floor(topLeft.x / tileSize));
  int startRow = static_cast<int>(std::floor(topLeft.y / tileSize));
  int endCol = static_cast<int>(std::ceil(bottomRight.x / tileSize));
  int endRow = static_cast<int>(std::ceil(bottomRight.y / tileSize));

  // ⭐ FIX: Origin harus di tile boundary (aligned ke tileSize)
  int originCol = static_cast<int>(std::floor(topLeft.x / tileSize));
  int originRow = static_cast<int>(std::floor(topLeft.y / tileSize));
  origin = vec2(originCol * tileSize, originRow * tileSize);

  // Generate grid positions (square grid)
  // ⭐ FIX: calculateGridOffset pakai relative indices (0-based)
  for (int row = startRow; row <= endRow; ++row) {
    for (int col = startCol; col <= endCol; ++col) {
      GridPosition pos;
      pos.row = row - startRow;  // Relative indices (0-based)
      pos.col = col - startCol;
      // ⭐ FIX: Pakai relative indices untuk offset
      pos.offset = calculateGridOffset(pos.row, pos.col, radius);
      grid.push_back(pos);
    }
  }

  isActive = true;
}

//--------------------------------------------------------------
void TessellationManager::clearGrid() {
  grid.clear();
  isActive = false;
  origin = vec2(0, 0);
}

//--------------------------------------------------------------
vec2 TessellationManager::rotatePoint(const vec2& point, float angle) {
  float rad = ofDegToRad(angle);
  float cosA = cos(rad);
  float sinA = sin(rad);
  return vec2(
    point.x * cosA - point.y * sinA,
    point.x * sinA + point.y * cosA
  );
}

//--------------------------------------------------------------
vec2 TessellationManager::calculateGridOffset(int row, int col, float radius) const {
  float tileSize = radius * 2.0f;
  float offsetX = origin.x + (col * tileSize);
  float offsetY = origin.y + (row * tileSize);
  return vec2(offsetX, offsetY);
}

//--------------------------------------------------------------
bool TessellationManager::isInViewport(const vec2& pos, const vec2& viewportSize) const {
  return (pos.x >= 0 && pos.x <= viewportSize.x &&
          pos.y >= 0 && pos.y <= viewportSize.y);
}
