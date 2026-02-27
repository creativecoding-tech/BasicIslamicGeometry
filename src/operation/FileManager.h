#pragma once

#include "../shape/CustomLine.h"
#include "../shape/DotShape.h"
#include "../shape/PolygonShape.h"
#include "../template/SacredGeometryTemplate.h"
#include "ofMain.h"
#include <string>

using glm::vec2;

// Enum untuk Polygon Animation Mode
enum class PolygonAnimationMode {
  NO_ANIMATION, // 0 - Tidak ada animasi
  FADE_IN,      // 1 - Alpha blending fade-in (0 → targetAlpha)
  WOBBLE,       // 2 - Polygon shape bergerak/bergoyang (vertices offset)
  WAVE_FILL, // 3 - Water fill effect dari bawah ke atas dengan wave horizontal
  WOBBLE_FILL, // 4 - Wobble effect pada warna fill (polygon statis, warna yang
               // wobble)
  GRADIENT     // 5 - Gradient flow effect pada warna fill (polygon statis,
               // pulsing/breathing)
};

class FileManager {
public:
  // Constructor
  FileManager();

  // Save/load ALL data ke .na format (CTRL+S / CTRL+O)
  void saveAll(const std::string &templateName, float globalRadius,
               const std::vector<CustomLine> &customLines,
               const std::vector<PolygonShape> &polygons,
               const std::vector<std::unique_ptr<DotShape>> &userDots,
               float currentLineWidth, bool labelsVisible, bool dotsVisible,
               bool showUserDot, const std::string &filepath);

  // Load ALL data dari .na format - return template name
  bool loadAll(std::string &outTemplateName, float &outGlobalRadius,
               std::vector<CustomLine> &customLines,
               std::vector<PolygonShape> &polygons,
               std::vector<std::unique_ptr<DotShape>> &userDots,
               float &outLineWidth, bool &outLabelsVisible,
               bool &outDotsVisible, bool &outShowUserDot,
               const std::string &filepath);

  // Load ALL data sequential dengan animasi (CTRL+SHIFT+O)
  void loadAllSequential(std::string &outTemplateName, float &outGlobalRadius,
                         float &outLineWidth, bool &outLabelsVisible,
                         bool &outDotsVisible,
                         std::vector<CustomLine> &customLines,
                         std::vector<PolygonShape> &polygons,
                         std::vector<std::unique_ptr<DotShape>> &userDots,
                         bool &outShowUserDot, const std::string &filepath);

  // Clear semua custom lines (CTRL+DEL)
  static void clearCustomLines(std::vector<CustomLine> &customLines);

  // Cancel sequential load (clear buffer dan reset state)
  void cancelSequentialLoad();

  // Update sequential load (dipanggil dari update())
  void updateSequentialLoad(std::vector<CustomLine> &customLines,
                            std::vector<PolygonShape> &polygons);

  // Getter untuk state
  bool isLoadSequentialMode() const;
  bool isLoadParallelMode() const;
  int getCurrentLoadIndex() const;
  int getTotalLoadedLines() const;
  int getTotalLoadedPolygons() const; // Getter untuk jumlah polygons di buffer
  int getTotalOriginalPolygons()
      const; // Getter untuk jumlah polygons NON-TESSELATED di buffer

  // Getters for buffers (used for instant access like tessellation)
  const std::vector<CustomLine> &getLoadedLinesBuffer() const {
    return loadedLinesBuffer;
  }
  const std::vector<PolygonShape> &getLoadedPolygonsBuffer() const {
    return loadedPolygonsBuffer;
  }
  std::vector<PolygonShape> &getLoadedPolygonsBufferRef() {
    return loadedPolygonsBuffer;
  } // ⭐ NEW

  // Setter untuk load mode (diperlukan untuk staggered load)
  void setLoadParallelMode(bool enabled);

  // Speed control
  void setLoadSpeed(float speed);
  float getLoadSpeed() const;

  // Animation speed multiplier control (untuk polygons & customLines)
  void setAnimationSpeedMultiplier(float multiplier);
  float getAnimationSpeedMultiplier() const;

  // Polygon animation mode control
  void setPolygonAnimationMode(PolygonAnimationMode mode);
  PolygonAnimationMode getPolygonAnimationMode() const;

  // Polygon Speed Multiplier (Controls Animation Speed & Load Interval)
  void setPolygonSpeedMultiplier(float multiplier);
  float getPolygonSpeedMultiplier() const;

  // CustomLines load control untuk sequential load
  void setShouldLoadCustomLines(bool shouldLoad);
  bool getShouldLoadCustomLines() const;

  // Helper method untuk create polygon dengan animation berdasarkan mode
  PolygonShape createPolygonWithAnimation(const std::vector<vec2> &vertices,
                                          ofColor color, int index);

  // Apply special polygon animations (No Animation / Rotate Left) ⭐ NEW
  void applySpecialPolygonAnimations(std::vector<PolygonShape> &polys,
                                     const std::vector<int> &specialAnims,
                                     const std::vector<float> &rotateAngles);

private:
  // Helper methods untuk .na format
  void saveCustomLinesNA(ofBuffer &buffer,
                         const std::vector<CustomLine> &customLines,
                         float radius);
  bool loadCustomLinesNA(ofBuffer &buffer, size_t &offset,
                         std::vector<CustomLine> &customLines, float radius,
                         int version);

  void savePolygonsNA(ofBuffer &buffer,
                      const std::vector<PolygonShape> &polygons, float radius);
  bool loadPolygonsNA(ofBuffer &buffer, size_t &offset,
                      std::vector<PolygonShape> &polygons, float radius,
                      int version);

  void saveUserDotsNA(ofBuffer &buffer,
                      const std::vector<std::unique_ptr<DotShape>> &userDots,
                      float radius);
  bool loadUserDotsNA(ofBuffer &buffer, size_t &offset,
                      std::vector<std::unique_ptr<DotShape>> &userDots,
                      float radius);

  // Load state flags
  bool loadSequentialMode;
  bool loadParallelMode;

  // Sequential load state untuk CustomLines
  std::vector<CustomLine> loadedLinesBuffer;
  int currentLineIndex;
  float loadSpeed;
  float loadAccumulator;

  // Sequential load state untuk Polygons
  std::vector<PolygonShape> loadedPolygonsBuffer;
  int currentPolygonIndex;

  // Polygon animation mode
  PolygonAnimationMode polygonAnimationMode;

  // Polygon speed multiplier (controls animation speed & load interval)
  float polygonSpeedMultiplier = 1.0f;

  // Should load customLines flag (untuk Draw Custom Lines checkbox)
  bool shouldLoadCustomLines = true;

  // Animation speed multiplier (untuk polygons & customLines)
  float animationSpeedMultiplier;

  // Format constants
  static constexpr char MAGIC_NUMBER[4] = {'N', 'A', '0', '1'};
  // Version file format
  static constexpr int VERSION = 4;
  static constexpr int HEADER_SIZE =
      64; // Header tetap 64 bytes untuk masa depan
};
