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

  // ⭐ NEW: Ring grouping untuk Radial Expansion mode
  struct RingInfo {
    int ringIndex;                  // Ring distance (0 = center, 1 = ring 1, dst)
    std::vector<size_t> tileIndices; // Indices ke grid vector untuk tiles di ring ini
    float delayStartTime;           // Waktu delay sebelum ring ini mulai animate
    bool isAnimating;               // True jika ring ini sedang animasi
    bool isComplete;                // True jika ring ini selesai animasi
    float elapsedTime;              // ⭐ NEW: Waktu yang telah berlalu sejak ring mulai animate (detik)
  };
  std::vector<RingInfo> rings;     // Rings untuk radial expansion (di-populate saat generateGrid)
  int centerRow, centerCol;        // Position dari center tile

  // ⭐ NEW: Diagonal grouping untuk Diagonal Expansion mode
  struct DiagonalInfo {
    int diagonalIndex;              // Diagonal index (0, 1, 2, ...) dari kiri-atas
    std::vector<size_t> tileIndices; // Indices ke grid vector untuk tiles di diagonal ini
  };
  std::vector<DiagonalInfo> diagonals; // Diagonals untuk diagonal expansion

  // ⭐ NEW: Row grouping untuk Seq Per Row mode
  struct RowInfo {
    int rowIndex;                   // Row index (0, 1, 2, ...) dari atas
    std::vector<size_t> tileIndices; // Indices ke grid vector untuk tiles di row ini
  };
  std::vector<RowInfo> rows;        // Rows untuk seq per row expansion

  // ⭐ NEW: Radial Expansion state
  bool isRadialExpansionActive;    // True jika Radial Expansion mode aktif
  int currentAnimatingRing;         // Ring yang sedang animasi (index ke rings vector)
  float ringAnimationProgress;      // Progress animasi ring saat ini (0.0 - 1.0)

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

  // ⭐ NEW: Distance calculation methods untuk Radial Expansion
  enum DistanceMethod {
    MANHATTAN,   // Diamond pattern: |row - centerRow| + |col - centerCol|
    EUCLIDEAN,    // Circular pattern: √((row - centerRow)² + (col - centerCol)²)
    CHEBYSHEV     // Square pattern: max(|row - centerRow|, |col - centerCol|)
  };

  // Hitung distance dari tile ke center grid
  int getDistanceFromCenter(int row, int col, int centerRow, int centerCol, DistanceMethod method) const;

  // ⭐ NEW: Ring grouping methods untuk Radial Expansion
  void groupTilesByDistance(DistanceMethod method);  // Group tiles into rings
  void calculateCenterTile();                          // Hitung center tile position
  int getRingCount() const { return static_cast<int>(rings.size()); }  // Get total rings

  // ⭐ NEW: Radial Expansion animation control
  void startRadialExpansion(float ringDuration);  // Mulai radial expansion mode
  void updateRadialExpansion(float deltaTime, float ringDuration);  // Update progress
  bool isRadialExpansionComplete() const;          // Cek apakah semua rings selesai
  void resetRadialExpansion();                       // Reset state
  float getRingElapsedTime(int ringIndex) const;   // ⭐ NEW: Get elapsed time untuk specific ring (untuk virtual time draw)

  // ⭐ NEW: Diagonal Expansion methods
  void groupTilesByDiagonal();  // Group tiles into diagonals (dari kiri-atas)
  int getDiagonalCount() const { return static_cast<int>(diagonals.size()); }  // Get total diagonals

  // ⭐ NEW: Seq Per Row methods
  void groupTilesByRow();       // Group tiles into rows (dari atas ke bawah)
  int getRowCount() const { return static_cast<int>(rows.size()); }  // Get total rows

  // Cek apakah posisi di dalam viewport bounds
  bool isInViewport(const vec2& pos, const vec2& viewportSize) const;
};
