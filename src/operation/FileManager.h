#pragma once

#include "ofMain.h"
#include "../shape/AbstractShape.h"
using glm::vec2;

// Struct untuk CustomLine
struct CustomLine {
    vector<vec2> points;  // Polyline dengan banyak titik (bukan cuma fromPos/toPos)
    ofColor color;
    float lineWidth;

    // Animation progress (0.0 - 1.0) untuk sequential drawing
    // Tidak disimpan ke file, hanya untuk runtime
    float progress = 1.0f;  // Default 1.0 = sudah lengkap
    float speed = 0.02f;    // Kecepatan animasi per line
};

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
    int getCurrentLoadIndex() const;
    int getTotalLoadedLines() const;

    // Speed control
    void setLoadSpeed(float speed);
    float getLoadSpeed() const;

private:
    // Sequential load state
    bool loadSequentialMode;
    std::vector<CustomLine> loadedLinesBuffer;
    int currentLoadIndex;
    float loadSpeed;
    float loadAccumulator;

    // Konstanta
    static const std::string FILENAME;
};
