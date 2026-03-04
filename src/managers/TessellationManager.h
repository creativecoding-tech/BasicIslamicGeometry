#pragma once

#include "ofMain.h"
#include "../template/SacredGeometryTemplate.h"
#include "../shape/CustomLine.h"
#include "../shape/PolygonShape.h"
#include <vector>

using glm::vec2;

//--------------------------------------------------------------
// TessellationManager - Handle canvas tessellation (square grid tiling)
// Men-copy pattern (template + custom lines + polygons) ke grid untuk menutup canvas
//--------------------------------------------------------------
class TessellationManager {
public:
  TessellationManager();

  // Grid position info untuk setiap tile
  struct GridPosition {
    int row, col;          // Grid index
    vec2 offset;           // World position offset dari origin
  };

  // Tessellation state
  std::vector<GridPosition> grid;  // Grid positions
  bool isActive = false;           // True sedang menampilkan tessellation grid
  vec2 origin;                     // Origin point grid (top-left viewport)

  // Tessellation settings
  float tessellationRadius = 120.0f;  // Radius untuk tessellation

  // Generate tessellation grid (square grid)
  // viewportSize: Ukuran viewport dalam pixels (screen space)
  // canvasTransform: Canvas transform parameters (translation, rotation, zoom)
  void generateGrid(float radius, const vec2& viewportSize, const vec2& canvasTranslation, float canvasRotation, float canvasZoom);

  // Clear tessellation grid
  void clearGrid();

  // Get grid count
  size_t getGridCount() const { return grid.size(); }

  // Hitung offset untuk grid position
  vec2 calculateGridOffset(int row, int col, float radius) const;

  // Helper: Rotate point around origin by angle (degrees)
  static vec2 rotatePoint(const vec2& point, float angle);

  // Cek apakah posisi di dalam viewport bounds
  bool isInViewport(const vec2& pos, const vec2& viewportSize) const;
};
