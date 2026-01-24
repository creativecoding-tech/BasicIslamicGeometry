#include "FileManager.h"

// Konstanta filename
const std::string FileManager::FILENAME = "custom_lines.bin";

//--------------------------------------------------------------
FileManager::FileManager()
    : loadSequentialMode(false)
    , currentLoadIndex(0)
    , loadSpeed(0.05f)
    , loadAccumulator(0.0f) {
}

//--------------------------------------------------------------
void FileManager::saveCustomLines(const std::vector<CustomLine>& customLines) {
    // Cek apakah ada lines untuk disimpan
    if (customLines.empty()) {
        return;  // Tidak create file jika kosong
    }

    ofBuffer buffer;

    // Write jumlah line dulu
    int numLines = static_cast<int>(customLines.size());
    buffer.append(reinterpret_cast<const char*>(&numLines), sizeof(int));

    // Write setiap line dalam binary format
    for (const auto& line : customLines) {
        // Write jumlah points dalam line ini
        const vector<vec2>& points = line.getPoints();
        int numPoints = static_cast<int>(points.size());
        buffer.append(reinterpret_cast<const char*>(&numPoints), sizeof(int));

        // Write semua points
        for (const auto& point : points) {
            buffer.append(reinterpret_cast<const char*>(&point), sizeof(vec2));
        }

        // Write color, lineWidth, dan curve
        ofColor color = line.getColor();
        float lineWidth = line.getLineWidth();
        float curve = line.getCurve();
        buffer.append(reinterpret_cast<const char*>(&color), sizeof(ofColor));
        buffer.append(reinterpret_cast<const char*>(&lineWidth), sizeof(float));
        buffer.append(reinterpret_cast<const char*>(&curve), sizeof(float));
    }

    // Write buffer ke file (selalu replace/overwrite)
    ofBufferToFile(FILENAME, buffer);
}

//--------------------------------------------------------------
bool FileManager::loadCustomLines(std::vector<CustomLine>& customLines) {
    // Cek apakah ada isinya dulu (proteksi kerjaan yang sudah dibuat)
    if (!customLines.empty()) {
        return false;  // Tidak load jika sudah ada isinya
    }

    // Cek apakah file exists
    ofFile file(FILENAME);
    if (!file.exists()) {
        return false;
    }

    // Read file ke buffer
    ofBuffer buffer = ofBufferFromFile(FILENAME);
    char* data = buffer.getData();

    // Read jumlah line
    int numLines = *reinterpret_cast<int*>(data);
    data += sizeof(int);

    // Clear existing lines
    customLines.clear();

    // Read setiap line
    for (int i = 0; i < numLines; i++) {
        CustomLine line;

        // Read jumlah points
        int numPoints = *reinterpret_cast<int*>(data);
        data += sizeof(int);

        // Read semua points
        vector<vec2> points;
        for (int j = 0; j < numPoints; j++) {
            vec2 point = *reinterpret_cast<vec2*>(data);
            data += sizeof(vec2);
            points.push_back(point);
        }
        line.setPoints(points);

        // Read color
        ofColor color = *reinterpret_cast<ofColor*>(data);
        data += sizeof(ofColor);
        line.setColor(color);

        // Read lineWidth
        float lineWidth = *reinterpret_cast<float*>(data);
        data += sizeof(float);
        line.setLineWidth(lineWidth);

        // Read curve
        float curve = *reinterpret_cast<float*>(data);
        data += sizeof(float);
        line.setCurve(curve);

        // Set progress ke 1.0 (langsung lengkap, bukan animasi)
        line.setProgress(1.0f);

        // Add ke vector
        customLines.push_back(line);
    }

    return true;
}

//--------------------------------------------------------------
void FileManager::loadCustomLinesSequential(std::vector<CustomLine>& customLines) {
    // Cek apakah ada isinya dulu (proteksi kerjaan yang sudah dibuat)
    if (!customLines.empty()) {
        return;
    }

    // Cek apakah file exists
    ofFile file(FILENAME);
    if (!file.exists()) {
        return;
    }

    // Baca file ke buffer
    ofBuffer buffer = ofBufferFromFile(FILENAME);
    char* data = buffer.getData();

    // Read jumlah line
    int numLines = *reinterpret_cast<int*>(data);
    data += sizeof(int);

    // Simpan SEMUA lines ke loadedLinesBuffer
    loadedLinesBuffer.clear();

    for (int i = 0; i < numLines; i++) {
        CustomLine line;

        // Read jumlah points
        int numPoints = *reinterpret_cast<int*>(data);
        data += sizeof(int);

        // Read semua points
        vector<vec2> points;
        for (int j = 0; j < numPoints; j++) {
            vec2 point = *reinterpret_cast<vec2*>(data);
            data += sizeof(vec2);
            points.push_back(point);
        }
        line.setPoints(points);

        // Read color
        ofColor color = *reinterpret_cast<ofColor*>(data);
        data += sizeof(ofColor);
        line.setColor(color);

        // Read lineWidth
        float lineWidth = *reinterpret_cast<float*>(data);
        data += sizeof(float);
        line.setLineWidth(lineWidth);

        // Read curve
        float curve = *reinterpret_cast<float*>(data);
        data += sizeof(float);
        line.setCurve(curve);

        // Set initial animation state
        line.setProgress(0.0f);  // Mulai dari 0 (belum tergambar)
        line.setSpeed(0.02f);     // Kecepatan animasi

        // Add ke buffer (BUKAN ke customLines!)
        loadedLinesBuffer.push_back(line);
    }

    // Reset index dan mulai mode sequential
    currentLoadIndex = 0;
    loadAccumulator = 0.0f;
    loadSequentialMode = true;
}

//--------------------------------------------------------------
void FileManager::clearCustomLines(std::vector<CustomLine>& customLines) {
    customLines.clear();
}

//--------------------------------------------------------------
void FileManager::updateSequentialLoad(std::vector<CustomLine>& customLines) {
    if (!loadSequentialMode) {
        return;
    }

    // Add line baru jika line terakhir sudah complete
    bool canAddNewLine = false;
    if (customLines.empty()) {
        canAddNewLine = true;
    } else {
        // Cek apakah line terakhir sudah complete
        CustomLine& lastLine = customLines.back();
        if (lastLine.getProgress() >= 1.0f && currentLoadIndex < static_cast<int>(loadedLinesBuffer.size())) {
            canAddNewLine = true;
        }
    }

    // Add line baru dengan delay
    if (canAddNewLine) {
        loadAccumulator += loadSpeed;
        if (loadAccumulator >= 1.0f && currentLoadIndex < static_cast<int>(loadedLinesBuffer.size())) {
            customLines.push_back(loadedLinesBuffer[currentLoadIndex]);
            currentLoadIndex++;
            loadAccumulator = 0.0f;
        }
    }

    // Update progress untuk semua lines yang belum complete
    for (auto& line : customLines) {
        line.updateProgress();
    }

    // Cek apakah sudah selesai (semua lines loaded dan semua complete)
    bool allComplete = true;
    if (currentLoadIndex < static_cast<int>(loadedLinesBuffer.size())) {
        allComplete = false;  // Masih ada lines yang belum di-load
    } else {
        // Cek apakah semua lines sudah complete
        for (const auto& line : customLines) {
            if (line.getProgress() < 1.0f) {
                allComplete = false;
                break;
            }
        }
    }

    if (allComplete) {
        loadSequentialMode = false;  // Selesai
        loadedLinesBuffer.clear();     // Bersihkan buffer
        loadAccumulator = 0.0f;        // Reset accumulator
    }
}

//--------------------------------------------------------------
bool FileManager::isLoadSequentialMode() const {
    return loadSequentialMode;
}

//--------------------------------------------------------------
int FileManager::getCurrentLoadIndex() const {
    return currentLoadIndex;
}

//--------------------------------------------------------------
int FileManager::getTotalLoadedLines() const {
    return static_cast<int>(loadedLinesBuffer.size());
}

//--------------------------------------------------------------
void FileManager::setLoadSpeed(float speed) {
    loadSpeed = speed;
}

//--------------------------------------------------------------
float FileManager::getLoadSpeed() const {
    return loadSpeed;
}
