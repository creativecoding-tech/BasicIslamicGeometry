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

  // ⭐ NEW: Clear rings dan reset radial expansion state
  rings.clear();
  resetRadialExpansion();

  // ⭐ NEW: Clear diagonals
  diagonals.clear();

  // ⭐ NEW: Clear rows
  rows.clear();
}

//--------------------------------------------------------------
void TessellationManager::resetRadialExpansion() {
  isRadialExpansionActive = false;
  currentAnimatingRing = 0;
  ringAnimationProgress = 0.0f;

  // Reset ring states
  for (auto& ring : rings) {
    ring.isAnimating = false;
    ring.isComplete = false;
    ring.delayStartTime = 0.0f;
    ring.elapsedTime = 0.0f;  // ⭐ Reset elapsed time
  }
}

//--------------------------------------------------------------
void TessellationManager::startRadialExpansion(float ringDuration) {
  if (rings.empty()) {
    // ofLog() << "Warning: startRadialExpansion called but no rings!";
    return;
  }

  // Reset state
  resetRadialExpansion();
  isRadialExpansionActive = true;
  currentAnimatingRing = 0;

  // Hitung delay start time untuk setiap ring
  float currentTime = 0.0f;
  for (size_t i = 0; i < rings.size(); ++i) {
    rings[i].delayStartTime = currentTime;
    rings[i].isAnimating = (i == 0);  // Hanya ring 0 yang mulai animasi
    rings[i].isComplete = false;
    rings[i].elapsedTime = 0.0f;       // ⭐ Initialize elapsed time

    // Next ring mulai setelah ring ini selesai (ringDuration detik)
    currentTime += ringDuration;
  }

  // ofLog() << "Radial Expansion Started - Total rings: " << rings.size() << ", Duration per ring: " << ringDuration;
}

//--------------------------------------------------------------
void TessellationManager::updateRadialExpansion(float deltaTime, float ringDuration) {
  if (!isRadialExpansionActive || rings.empty()) {
    return;
  }

  // Cek apakah current ring sudah selesai
  if (currentAnimatingRing >= rings.size()) {
    // Semua rings selesai
    isRadialExpansionActive = false;
    // ofLog() << "Radial Expansion Complete!";
    return;
  }

  RingInfo& currentRing = rings[currentAnimatingRing];

  // Cek apakah sudah waktunya ring ini mulai
  if (!currentRing.isAnimating) {
    // Cek apakah delay time sudah lewat
    // Ring N mulai setelah ring N-1 complete
    if (currentAnimatingRing == 0 || rings[currentAnimatingRing - 1].isComplete) {
      currentRing.isAnimating = true;
      currentRing.elapsedTime = 0.0f; // Reset elapsed time saat mulai
      // ofLog() << "Ring " << currentAnimatingRing << " started animating";
    }
  }

  // Update elapsed time ring yang sedang animasi
  if (currentRing.isAnimating && !currentRing.isComplete) {
    currentRing.elapsedTime += deltaTime;
    ringAnimationProgress = currentRing.elapsedTime / ringDuration;

    if (ringAnimationProgress >= 1.0f) {
      ringAnimationProgress = 1.0f;
      currentRing.isComplete = true;
      // ofLog() << "Ring " << currentAnimatingRing << " complete (elapsed: " << currentRing.elapsedTime << "s)";

      // Move to next ring
      currentAnimatingRing++;
      ringAnimationProgress = 0.0f;
    }
  }
}

//--------------------------------------------------------------
bool TessellationManager::isRadialExpansionComplete() const {
  if (!isRadialExpansionActive) {
    return false;
  }

  // Cek apakah semua rings complete
  for (const auto& ring : rings) {
    if (!ring.isComplete) {
      return false;
    }
  }

  return true;
}

//--------------------------------------------------------------
float TessellationManager::getRingElapsedTime(int ringIndex) const {
  if (ringIndex >= 0 && ringIndex < static_cast<int>(rings.size())) {
    return rings[ringIndex].elapsedTime;
  }
  return 0.0f;
}

//--------------------------------------------------------------
// ⭐ NEW: Diagonal Expansion methods
void TessellationManager::groupTilesByDiagonal() {
  diagonals.clear();

  if (grid.empty()) return;

  // Hitung maksimum diagonal index
  int maxDiagonal = 0;
  for (const auto& tile : grid) {
    int diagonal = tile.row + tile.col;
    if (diagonal > maxDiagonal) {
      maxDiagonal = diagonal;
    }
  }

  // Create diagonal info untuk setiap diagonal
  for (int d = 0; d <= maxDiagonal; ++d) {
    DiagonalInfo diag;
    diag.diagonalIndex = d;

    // Cari semua tiles yang ada di diagonal ini
    for (size_t i = 0; i < grid.size(); ++i) {
      if (grid[i].row + grid[i].col == d) {
        diag.tileIndices.push_back(i);
      }
    }

    diagonals.push_back(diag);
  }
}

//--------------------------------------------------------------
// ⭐ NEW: Seq Per Row methods
void TessellationManager::groupTilesByRow() {
  rows.clear();

  if (grid.empty()) return;

  // Hitung maksimum row index
  int maxRow = 0;
  for (const auto& tile : grid) {
    if (tile.row > maxRow) {
      maxRow = tile.row;
    }
  }

  // Create row info untuk setiap row
  for (int r = 0; r <= maxRow; ++r) {
    RowInfo rowInfo;
    rowInfo.rowIndex = r;

    // Cari semua tiles yang ada di row ini
    for (size_t i = 0; i < grid.size(); ++i) {
      if (grid[i].row == r) {
        rowInfo.tileIndices.push_back(i);
      }
    }

    rows.push_back(rowInfo);
  }
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
int TessellationManager::getDistanceFromCenter(int row, int col, int centerRow, int centerCol, DistanceMethod method) const {
  switch (method) {
    case MANHATTAN:
      // Diamond pattern: |row - centerRow| + |col - centerCol|
      return std::abs(row - centerRow) + std::abs(col - centerCol);

    case EUCLIDEAN:
      // Circular pattern: √((row - centerRow)² + (col - centerCol)²)
      return static_cast<int>(std::sqrt(std::pow(row - centerRow, 2) + std::pow(col - centerCol, 2)));

    case CHEBYSHEV:
      // Square pattern: max(|row - centerRow|, |col - centerCol|)
      return std::max(std::abs(row - centerRow), std::abs(col - centerCol));

    default:
      return 0;
  }
}

//--------------------------------------------------------------
void TessellationManager::calculateCenterTile() {
  // Center tile = tile di tengah-tengah grid
  // Untuk rectangular grid (bukan square), cari tile yang paling dekat ke center
  if (grid.empty()) {
    centerRow = 0;
    centerCol = 0;
    return;
  }

  // Hitung bounds grid
  int minRow = grid[0].row;
  int maxRow = grid[0].row;
  int minCol = grid[0].col;
  int maxCol = grid[0].col;

  for (const auto& tile : grid) {
    if (tile.row < minRow) minRow = tile.row;
    if (tile.row > maxRow) maxRow = tile.row;
    if (tile.col < minCol) minCol = tile.col;
    if (tile.col > maxCol) maxCol = tile.col;
  }

  // Center = middle of bounds
  centerRow = (minRow + maxRow) / 2;
  centerCol = (minCol + maxCol) / 2;
}

//--------------------------------------------------------------
void TessellationManager::groupTilesByDistance(DistanceMethod method) {
  // Clear existing rings
  rings.clear();

  // Pastikan center tile sudah dihitung
  if (grid.empty()) return;

  // Hitung center tile
  calculateCenterTile();

  // Group tiles by distance dari center
  std::map<int, std::vector<size_t>> distanceMap;  // distance -> tile indices

  for (size_t i = 0; i < grid.size(); ++i) {
    int distance = getDistanceFromCenter(grid[i].row, grid[i].col, centerRow, centerCol, method);
    distanceMap[distance].push_back(i);
  }

  // Convert map ke RingInfo vector
  int maxDistance = 0;
  if (!distanceMap.empty()) {
    maxDistance = distanceMap.rbegin()->first;
  }

  for (int dist = 0; dist <= maxDistance; ++dist) {
    if (distanceMap.find(dist) != distanceMap.end()) {
      RingInfo ring;
      ring.ringIndex = dist;
      ring.tileIndices = distanceMap[dist];
      ring.delayStartTime = 0.0f;  // Akan dihitung nanti berdasarkan ring duration
      rings.push_back(ring);
    }
  }

  // ofLog() << "Ring Grouping - Total rings: " << rings.size() << ", Center: (" << centerRow << "," << centerCol << ")";
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
