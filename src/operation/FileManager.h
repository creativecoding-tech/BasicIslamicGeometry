#pragma once

#include "ofMain.h"
#include "../shape/CustomLine.h"
#include "../shape/PolygonShape.h"
#include "../shape/DotShape.h"
#include "../template/SacredGeometryTemplate.h"
#include <string>
using glm::vec2;

// Enum untuk Polygon Animation Mode
enum class PolygonAnimationMode {
    NO_ANIMATION,  // 0
    FADE_IN,       // 1
    WOBBLE,        // 2
    FILL           // 3
};

class FileManager {
public:
    // Constructor
    FileManager();

    // Save/load ALL data ke .na format (CTRL+S / CTRL+O)
    void saveAll(const std::string& templateName, float globalRadius,
                 const std::vector<CustomLine>& customLines,
                 const std::vector<PolygonShape>& polygons,
                 const std::vector<std::unique_ptr<DotShape>>& userDots,
                 float currentLineWidth, bool labelsVisible, bool dotsVisible,
                 bool showUserDot, const std::string& filepath);

    // Load ALL data dari .na format - return template name
    bool loadAll(std::string& outTemplateName, float& outGlobalRadius,
                 std::vector<CustomLine>& customLines,
                 std::vector<PolygonShape>& polygons,
                 std::vector<std::unique_ptr<DotShape>>& userDots,
                 float& outLineWidth, bool& outLabelsVisible, bool& outDotsVisible,
                 bool& outShowUserDot, const std::string& filepath);

    // Load ALL data sequential dengan animasi (CTRL+SHIFT+O)
    void loadAllSequential(std::string& outTemplateName, float& outGlobalRadius,
                          float& outLineWidth, bool& outLabelsVisible, bool& outDotsVisible,
                          std::vector<CustomLine>& customLines, std::vector<PolygonShape>& polygons,
                          std::vector<std::unique_ptr<DotShape>>& userDots,
                          bool& outShowUserDot, const std::string& filepath);

    // Clear semua custom lines (CTRL+DEL)
    static void clearCustomLines(std::vector<CustomLine>& customLines);

    // Cancel sequential load (clear buffer dan reset state)
    void cancelSequentialLoad();

    // Update sequential load (dipanggil dari update())
    void updateSequentialLoad(std::vector<CustomLine>& customLines, std::vector<PolygonShape>& polygons);

    // Getter untuk state
    bool isLoadSequentialMode() const;
    bool isLoadParallelMode() const;
    int getCurrentLoadIndex() const;
    int getTotalLoadedLines() const;

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

private:
    // Helper methods untuk .na format
    void saveCustomLinesNA(ofBuffer& buffer, const std::vector<CustomLine>& customLines, float radius);
    bool loadCustomLinesNA(ofBuffer& buffer, size_t& offset, std::vector<CustomLine>& customLines, float radius);

    void savePolygonsNA(ofBuffer& buffer, const std::vector<PolygonShape>& polygons, float radius);
    bool loadPolygonsNA(ofBuffer& buffer, size_t& offset, std::vector<PolygonShape>& polygons, float radius);

    void saveUserDotsNA(ofBuffer& buffer, const std::vector<std::unique_ptr<DotShape>>& userDots, float radius);
    bool loadUserDotsNA(ofBuffer& buffer, size_t& offset, std::vector<std::unique_ptr<DotShape>>& userDots, float radius);

    // Helper method untuk create polygon dengan animation berdasarkan mode
    PolygonShape createPolygonWithAnimation(const std::vector<vec2>& vertices, ofColor color, int index);

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

    // Animation speed multiplier (untuk polygons & customLines)
    float animationSpeedMultiplier;

    // Format constants
    static constexpr char MAGIC_NUMBER[4] = {'N', 'A', '0', '1'};
    static constexpr int VERSION = 2;
    static constexpr int HEADER_SIZE = 64;  // Header tetap 64 bytes untuk masa depan
};
