#pragma once

#include "ofMain.h"
#include "../shape/CustomLine.h"
using glm::vec2;

class FileManager {
public:
    // Constructor
    FileManager();

    // Save custom lines ke binary file (CTRL+S)
    void saveCustomLines(const std::vector<CustomLine>& customLines);

    // Load semua custom lines sekaligus (CTRL+O)
    bool loadCustomLines(std::vector<CustomLine>& customLines);

    // Load custom lines sequential dengan animasi (CTRL+SHIFT+O)
    void loadCustomLinesSequential(std::vector<CustomLine>& customLines);

    // Clear semua custom lines (CTRL+DEL)
    static void clearCustomLines(std::vector<CustomLine>& customLines);

    // Update sequential load (dipanggil dari update())
    void updateSequentialLoad(std::vector<CustomLine>& customLines);

    // Getter untuk state
    bool isLoadSequentialMode() const;
    bool isLoadParallelMode() const;
    int getCurrentLoadIndex() const;
    int getTotalLoadedLines() const;

    // Speed control
    void setLoadSpeed(float speed);
    float getLoadSpeed() const;

private:
    // Load state flags
    bool loadSequentialMode;
    bool loadParallelMode;

    // Sequential load state
    std::vector<CustomLine> loadedLinesBuffer;
    int currentLoadIndex;
    float loadSpeed;
    float loadAccumulator;

    // Konstanta
    static const std::string FILENAME;
};
