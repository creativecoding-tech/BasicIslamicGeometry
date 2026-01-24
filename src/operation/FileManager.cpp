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
        int numPoints = static_cast<int>(line.points.size());
        buffer.append(reinterpret_cast<const char*>(&numPoints), sizeof(int));

        // Write semua points
        for (const auto& point : line.points) {
            buffer.append(reinterpret_cast<const char*>(&point), sizeof(vec2));
        }

        // Write color dan lineWidth
        buffer.append(reinterpret_cast<const char*>(&line.color), sizeof(ofColor));
        buffer.append(reinterpret_cast<const char*>(&line.lineWidth), sizeof(float));
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
        for (int j = 0; j < numPoints; j++) {
            vec2 point = *reinterpret_cast<vec2*>(data);
            data += sizeof(vec2);
            line.points.push_back(point);
        }

        // Read color
        line.color = *reinterpret_cast<ofColor*>(data);
        data += sizeof(ofColor);

        // Read lineWidth
        line.lineWidth = *reinterpret_cast<float*>(data);
        data += sizeof(float);

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
        for (int j = 0; j < numPoints; j++) {
            vec2 point = *reinterpret_cast<vec2*>(data);
            data += sizeof(vec2);
            line.points.push_back(point);
        }

        // Read color
        line.color = *reinterpret_cast<ofColor*>(data);
        data += sizeof(ofColor);

        // Read lineWidth
        line.lineWidth = *reinterpret_cast<float*>(data);
        data += sizeof(float);

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

    // Akumulasi speed
    loadAccumulator += loadSpeed;

    // Ambil bagian integer dari accumulator
    int linesToLoad = static_cast<int>(loadAccumulator);

    // Kurangi accumulator dengan yang sudah diambil
    loadAccumulator -= linesToLoad;

    // Load lines
    for (int i = 0; i < linesToLoad && currentLoadIndex < loadedLinesBuffer.size(); i++) {
        customLines.push_back(loadedLinesBuffer[currentLoadIndex]);
        currentLoadIndex++;
    }

    // Cek apakah sudah selesai
    if (currentLoadIndex >= static_cast<int>(loadedLinesBuffer.size())) {
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
