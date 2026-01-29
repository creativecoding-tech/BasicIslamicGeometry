#pragma once

#include "ofMain.h"
#include "../shape/CustomLine.h"
#include "../shape/PolygonShape.h"
#include "../template/SacredGeometryTemplate.h"
#include <string>
using glm::vec2;

class FileManager {
public:
    // Constructor
    FileManager();

    // Save/load ALL data ke .na format (CTRL+S / CTRL+O)
    void saveAll(const std::string& templateName, float globalRadius,
                 const std::vector<CustomLine>& customLines,
                 const std::vector<PolygonShape>& polygons,
                 float currentLineWidth, bool labelsVisible, bool dotsVisible);

    // Load ALL data dari .na format - return template name
    bool loadAll(std::string& outTemplateName, float& outGlobalRadius,
                 std::vector<CustomLine>& customLines,
                 std::vector<PolygonShape>& polygons,
                 float& outLineWidth, bool& outLabelsVisible, bool& outDotsVisible,
                 const std::string& filepath);

    // Load ALL data sequential dengan animasi (CTRL+SHIFT+O)
    void loadAllSequential(std::string& outTemplateName, float& outGlobalRadius,
                          float& outLineWidth, bool& outLabelsVisible, bool& outDotsVisible,
                          std::vector<CustomLine>& customLines, std::vector<PolygonShape>& polygons,
                          const std::string& filepath);

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

private:
    // Helper methods untuk .na format
    void saveCustomLinesNA(ofBuffer& buffer, const std::vector<CustomLine>& customLines, float radius);
    bool loadCustomLinesNA(ofBuffer& buffer, size_t& offset, std::vector<CustomLine>& customLines, float radius);

    void savePolygonsNA(ofBuffer& buffer, const std::vector<PolygonShape>& polygons, float radius);
    bool loadPolygonsNA(ofBuffer& buffer, size_t& offset, std::vector<PolygonShape>& polygons, float radius);

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

    // Konstanta
    static const std::string FILENAME;

    // Format constants
    static constexpr char MAGIC_NUMBER[4] = {'N', 'A', '0', '1'};
    static constexpr int VERSION = 2;
    static constexpr int HEADER_SIZE = 64;  // Header tetap 64 bytes untuk masa depan
};
