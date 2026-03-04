#include "TessellationManager.h"

//--------------------------------------------------------------
TessellationManager::TessellationManager() {
  origin = vec2(0, 0);
  isActive = false;
  tessellationRadius = 120.0f;
}

//--------------------------------------------------------------
void TessellationManager::generateGrid(float radius, const vec2& viewportSize) {
  tessellationRadius = radius;

  // Clear existing grid
  grid.clear();

  // Hitung tile size (square grid)
  float tileSize = radius * 2.0f;

  // Hitung berapa tile untuk menutup viewport (dengan margin)
  // Margin 1 tile extra di setiap sisi untuk coverage penuh
  int tilesX = static_cast<int>(std::ceil(viewportSize.x / tileSize)) + 2;
  int tilesY = static_cast<int>(std::ceil(viewportSize.y / tileSize)) + 2;

  // Origin grid (top-left) - geser ke luar viewport
  // Supaya viewport tertutup penuh
  float originX = -tileSize;
  float originY = -tileSize;
  origin = vec2(originX, originY);

  // Generate grid positions (square grid)
  for (int row = 0; row < tilesY; ++row) {
    for (int col = 0; col < tilesX; ++col) {
      GridPosition pos;
      pos.row = row;
      pos.col = col;
      pos.offset = calculateGridOffset(row, col, radius);
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
