#pragma once

#include "ofMain.h"
#include "../shape/CustomLine.h"
#include "../shape/PolygonShape.h"
using glm::vec2;

class FileManager {
public:
    // Constructor
    FileManager();

    // Save/load ALL data (CTRL+S / CTRL+O)
    void saveAll(const std::vector<CustomLine>& customLines, const std::vector<PolygonShape>& polygons);
    bool loadAll(std::vector<CustomLine>& customLines, std::vector<PolygonShape>& polygons);

    // Load ALL data sequential dengan animasi (CTRL+SHIFT+O)
    void loadAllSequential(std::vector<CustomLine>& customLines, std::vector<PolygonShape>& polygons);

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

    // Speed control
    void setLoadSpeed(float speed);
    float getLoadSpeed() const;

private:
    // Helper methods untuk save/load customLines
    void saveCustomLines(const std::vector<CustomLine>& customLines);
    bool loadCustomLines(std::vector<CustomLine>& customLines);

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
    static const std::string FILENAME_POLYGONS;
};
